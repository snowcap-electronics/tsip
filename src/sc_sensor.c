/*
 * Copyright 2019 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sc_sensor.h"

#ifdef CONFIG_SENSOR

#ifndef SC_SENSOR_MAX
#define SC_SENSOR_MAX 2
#endif

#ifndef SC_SENSOR_CHANNEL_MAX
#define SC_SENSOR_CHANNEL_MAX 4
#endif

#ifndef SC_SENSOR_THREAD_STACK_SIZE
#define SC_SENSOR_THREAD_STACK_SIZE 512
#endif

typedef struct sc_sensor {
  bool enabled;
  uint64_t next_poll_time;
  struct device *dev;
  enum sensor_channel channels[SC_SENSOR_CHANNEL_MAX];
  uint8_t channel_count;
  uint32_t interval_ms;
  const char *event_name;
  uint8_t event_type;
  struct sensor_value values[SC_SENSOR_CHANNEL_MAX];
} sc_sensor;


static sc_sensor sc_sensor_list[SC_SENSOR_MAX];

K_MUTEX_DEFINE(sc_sensor_data_mutex);

void sc_sensor_loop(void *, void *, void *);

K_THREAD_DEFINE(sc_sensor_thread, SC_SENSOR_THREAD_STACK_SIZE,
                sc_sensor_loop, NULL, NULL, NULL,
                SC_THREAD_PRIORITY_MEDIUM, 0, K_NO_WAIT);

static bool sc_sensor_read(sc_sensor *s);

/*
 * Sensor polling thread
 */
void sc_sensor_loop(void *arg1, void *arg2, void *arg3)
{
  (void)arg1;
  (void)arg2;
  (void)arg3;

  while (true) {
    s64_t now = k_uptime_get();
    s64_t next_poll_time = now + 24*60*60*1000; // Sleep 24 hrs by default

    k_mutex_lock(&sc_sensor_data_mutex, K_FOREVER);

    for (uint8_t i = 0; i < SC_SENSOR_MAX; ++i) {
      sc_sensor *s = &sc_sensor_list[i];
      if (s->enabled == false) {
        continue;
      }

      if (now >= s->next_poll_time) {
        s->next_poll_time += s->interval_ms;
        sc_sensor_read(s);
      }

      if (s->next_poll_time < next_poll_time) {
        next_poll_time = s->next_poll_time;
      }
    }

    k_mutex_unlock(&sc_sensor_data_mutex);

    // No need to worry about getting woken up too soon, the while loop will
    // just cause a new sleep.
    s64_t sleep_time = next_poll_time - now;
    //printk("Sleeping %lld ms until %lld\n", sleep_time, next_poll_time);
    k_sleep(sleep_time);
  }
}


/*
 * Initialize the sensor thread.
 * Returns true on success
 */
bool sc_sensor_init(void)
{
  // Nothing to do here.

  return true;
}


/*
 * Add sensor to the list of sensor to be periodically polled.
 * Returns true on success.
 */
bool sc_sensor_add(const char *device_name,
                   enum sensor_channel *channels,
                   uint8_t channel_count,
                   uint32_t interval_ms,
                   const char *event_name,
                   uint8_t *index)
{
  k_mutex_lock(&sc_sensor_data_mutex, K_FOREVER);

  for (uint8_t i = 0; i < SC_SENSOR_MAX; ++i) {
    sc_sensor *s = &sc_sensor_list[i];
    if (s->enabled == true) {
      continue;
    }

    struct device *dev = device_get_binding(device_name);
    if (dev == NULL) {
      // TODO: log an error
      k_mutex_unlock(&sc_sensor_data_mutex);
      return false;
    }

    s->dev = dev;
    // TODO: do we need to check that channel_count > 8?
    s->channel_count = channel_count;
    for (uint8_t c = 0; c < channel_count; ++c) {
      s->channels[c] = channels[c];
    }
    s->interval_ms = interval_ms;
    s->event_name = event_name;
    s->next_poll_time = k_uptime_get() + interval_ms;
    s->enabled = true;

    // Register test event type and callback
    if (!sc_event_register_type(s->event_name, &s->event_type)){
      k_mutex_unlock(&sc_sensor_data_mutex);
      return false;
    }

    k_mutex_unlock(&sc_sensor_data_mutex);
    return true;
  }
  k_mutex_unlock(&sc_sensor_data_mutex);

  // TODO: log an overflow error
  return false;
}

/*
 * Get the latest sensor read value.
 *
 * Returns true on success.
 */
bool sc_sensor_get_value(uint8_t index, struct sensor_value *values)
{
  // TODO: add boundary check for index
  // TODO: add null check for value

  sc_sensor *s = &sc_sensor_list[index];

  // Note: Once enabled, the sensor cannot be ever disabled. No need to lock the mutex for this check
  if (s->enabled == false) {
    // TODO: report an error?
    return false;
  }

  k_mutex_lock(&sc_sensor_data_mutex, K_FOREVER);
  for (uint8_t c = 0; c < s->channel_count; ++c) {
    values[c] = s->values[c];
  }
  k_mutex_unlock(&sc_sensor_data_mutex);

  return true;
}



int sc_sensor_value_snprintf(char *buf, size_t len,
                             const struct sensor_value *val)
{
  s32_t val1, val2;

  if (val->val2 == 0) {
    return snprintf(buf, len, "%d", val->val1);
  }

  /* normalize value */
  if (val->val1 < 0 && val->val2 > 0) {
    val1 = val->val1 + 1;
    val2 = val->val2 - 1000000;
  } else {
    val1 = val->val1;
    val2 = val->val2;
  }

  /* print value to buffer */
  if (val1 > 0 || (val1 == 0 && val2 > 0)) {
    return snprintf(buf, len, "%d.%06d", val1, val2);
  } else if (val1 == 0 && val2 < 0) {
    return snprintf(buf, len, "-0.%06d", -val2);
  } else {
    return snprintf(buf, len, "%d.%06d", val1, -val2);
  }
}

/*
 * Read a single sensor for a single channel.
 *
 * Returns true on success
 */
static bool sc_sensor_read(sc_sensor *s)
{
  int ret = 0;

  //printk("Reading sensor\n");

  // Unlock during the possible slow sample fetch
  // Note: We can unlock here as a sensor cannot be changed. Only new
  // ones can be added.
  k_mutex_unlock(&sc_sensor_data_mutex);
  ret = sensor_sample_fetch(s->dev);
  k_mutex_lock(&sc_sensor_data_mutex, K_FOREVER);

  if (ret != 0) {
    // TODO: error logging
    printk("Failed to sample fetch: %d\n", ret);
    return false;
  }

  for (uint8_t c = 0; c < s->channel_count; ++c) {
    ret = sensor_channel_get(s->dev, s->channels[c], &s->values[c]);
    if (ret != 0) {
      // TODO: error logging
      printk("Failed to sample fetch: %d\n", ret);
      return false;
    }
  }


  if (!sc_event_post(s->event_type)) {
    // TODO: Error logging
    printk("Failed sc_event_post\n");
    return false;
  }

  return true;
}

#endif // CONFIG_SENSOR