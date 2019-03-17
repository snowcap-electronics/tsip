/*
 * Copyright 2018 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sc.h"
#include <misc/printk.h>
#include <sensor.h>

static void event_sht3x_cb(void);
static void main_halt(void);

/* NOTE! This list must be in the same order as the error list below */
typedef enum sc_error_main {
  SC_ERROR_MAIN_EVENT_POST_FAIL = 0,
  SC_ERROR_MAIN_COUNT,
} sc_error_main;

/* NOTE! This list must be in the same order as the enum above */
static sc_error_data sc_error_list1[SC_ERROR_MAIN_COUNT] = {
  {0, 0, "sc_event_post failed"},
};

static uint8_t sht3x_index;

int main(void)
{
  // Register error module
  uint8_t error_module;
  if (sc_error_register("MAIN", sc_error_list1, ARRAY_SIZE(sc_error_list1), &error_module) == false) {
    sc_halt();
  }

  // Register event type and callback for the SHT3X sensor
  if (sc_event_register_type_cb("sht3x", &event_sht3x_cb) == false){
    main_halt();
  }

  uint32_t subsystems = 0;
  if (sc_init(subsystems) == false) {
    main_halt();
  }

  // List of channels on SHT3X
  enum sensor_channel channels[] = {
    SENSOR_CHAN_AMBIENT_TEMP,
    SENSOR_CHAN_HUMIDITY,
  };

  // Start measuring SHT3X sensor
  if (!sc_sensor_add("SHT3XD", channels, ARRAY_SIZE(channels), 60*1000, "sht3x", &sht3x_index)) {
    //main_halt();
  }

  // Main thread will loop forever. Event thread will execute callbacks.
  while(1) {
    //printk("Main thread idling\n");
    k_sleep(2000);
  }

  return 0;
}

static void event_sht3x_cb(void)
{
  static uint64_t seq = 0;

  // TODO: should use ARRAY_SIZE() etc. to match with the sc_sensor_add()
  struct sensor_value values[2];
  char temp[16], hum[16];

  if (!sc_sensor_get_value(sht3x_index, values)) {
    // TODO: Error logging
    return;
  }

  sc_sensor_value_snprintf(temp, sizeof(temp), &values[0]);
  sc_sensor_value_snprintf(hum, sizeof(hum), &values[1]);

  printk("m: hum[0]: %llu, %s, %s, %d, %d\n", seq, temp, hum, 0, 0);
  ++seq;
}

// TODO: implement sc_halt with an application specific halt function?
static void main_halt(void)
{
  // TODO: Blink big red warning light? Reset after a while?
  while(1) {};
}
