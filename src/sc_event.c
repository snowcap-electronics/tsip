/*
 * Copyright 2018 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sc.h"

#include <string.h>

#ifndef SC_EVENT_MAX_TYPE
#define SC_EVENT_MAX_TYPE 16
#endif

#ifndef SC_EVENT_QUEUE_SIZE
#define SC_EVENT_QUEUE_SIZE 64
#endif

#ifndef SC_EVENT_THREAD_STACK_SIZE
#define SC_EVENT_THREAD_STACK_SIZE 2048
#endif

/* NOTE! This list must be in the same order as the error list below */
typedef enum sc_error_event {
  SC_ERROR_EVENT_K_MSGQ_GET = 0,
  SC_ERROR_EVENT_LOOP_UNKNOWN_EVENT,
  SC_ERROR_EVENT_POST_UNKNOWN_EVENT,
  SC_ERROR_EVENT_NO_CALLBACK,
  SC_ERROR_EVENT_MSGQ_PUT,
  SC_ERROR_EVENT_GET_INVALID_PARAMETER,
  SC_ERROR_EVENT_GET_MODULE_OVERFLOW,
  SC_ERROR_EVENT_NOT_INITIALIZED,
  SC_ERROR_EVENT_REGCB_MODULE_UNKNOWN,
  SC_ERROR_EVENT_REGCB_ALREADY_REGISTERED,
  SC_ERROR_EVENT_COUNT,
} sc_error_event;

/* NOTE! This list must be in the same order as the enum above */
static sc_error_data sc_error_list[SC_ERROR_EVENT_COUNT] = {
  {0, 0, "k_msgq_get failed"},
  {0, 0, "loop: unknown event"},
  {0, 0, "post: unknown event"},
  {0, 0, "no callback function set"},
  {0, 0, "k_msgq_put failed"},
  {0, 0, "get: invalid parameter"},
  {0, 0, "get: module overflow"},
  {0, 0, "not initialized"},
  {0, 0, "reg_cb: module overflow"},
  {0, 0, "reg_cb: already registered"},
};

static uint8_t error_module;

typedef struct sc_event_type_t {
  const char *name;
  sc_event_cb cb;
} sc_event_type;

static sc_event_type sc_event_type_list[SC_EVENT_MAX_TYPE];

K_MUTEX_DEFINE(sc_event_data_mutex);

K_MSGQ_DEFINE(sc_event_msgq, sizeof(uint32_t), SC_EVENT_QUEUE_SIZE, 4);

void sc_event_loop(void *, void *, void *);

K_THREAD_DEFINE(sc_event_thread, SC_EVENT_THREAD_STACK_SIZE,
                sc_event_loop, NULL, NULL, NULL,
                SC_THREAD_PRIORITY_HIGH, K_ESSENTIAL, K_NO_WAIT);

static bool is_initialized = false;

/*
 * Setup a working area for the event loop thread
 */
void sc_event_loop(void *arg1, void *arg2, void *arg3)
{
  (void)arg1;
  (void)arg2;
  (void)arg3;

  if (sc_error_register("EVENT", sc_error_list, ARRAY_SIZE(sc_error_list), &error_module) == false) {
    sc_halt();
  }

  /* TODO: Init not needed for event loop?
  * Maybe it's needed to verify that the loop thread is actually running.
  * Maybe it actually doesn't even need to be running before it can already be used
  * */
  while (is_initialized == false) {
    k_sleep(1);
  }

  while (true) {
    uint32_t type;

    int ret = k_msgq_get(&sc_event_msgq, &type, K_FOREVER);
    if (ret != 0) {
      sc_error_log(error_module, SC_ERROR_EVENT_K_MSGQ_GET);
      continue;
    }

    // TODO: use the real amount of registered events, not the allocated max
    if (type >= SC_EVENT_MAX_TYPE) {
      sc_error_log_int(error_module, SC_ERROR_EVENT_LOOP_UNKNOWN_EVENT, type);
      continue;
    }

    if (sc_event_type_list[type].cb == NULL) {
      sc_error_log(error_module, SC_ERROR_EVENT_NO_CALLBACK);
      continue;
    }

    sc_event_type_list[type].cb();
  }
}



/*
* Initialize the event loop
* Returns true on success
*/
bool sc_event_init(void)
{
  is_initialized = true;

  return true;
}



/*
 * Post an event.
 * Returns true on success
 */
bool sc_event_post(uint8_t type)
{
  if (is_initialized == false) {
    sc_error_log(error_module, SC_ERROR_EVENT_NOT_INITIALIZED);
    return false;
  }

  // TODO: use the real amount of registered events, not the allocated max
  if (type >= SC_EVENT_MAX_TYPE) {
    sc_error_log_int(error_module, SC_ERROR_EVENT_POST_UNKNOWN_EVENT, type);
    return false;
  }

  /* No need to post if there's nobody listening */
  if (!sc_event_type_list[type].cb) {
    return false;
  }

  uint32_t type_msg = type;
  int ret = k_msgq_put(&sc_event_msgq, &type_msg, K_NO_WAIT);
  if (ret != 0) {
    sc_error_log_int(error_module, SC_ERROR_EVENT_MSGQ_PUT, ret);
    return false;
  }

  return true;
}



/*
 * Query the type number for the given type name. Will be initialized on the first call.
 * Return true on success
 */
bool sc_event_register_type(const char *name, uint8_t *type)
{

  if (name == NULL || type == NULL) {
    sc_error_log(error_module, SC_ERROR_EVENT_GET_INVALID_PARAMETER);
    return false;
  }

  k_mutex_lock(&sc_event_data_mutex, K_FOREVER);
  for (uint8_t i = 0; i < SC_EVENT_MAX_TYPE; ++i) {

    // Found empty slot, register new
    if (sc_event_type_list[i].name == NULL) {
      sc_event_type_list[i].name = name;
      *type = i;
      k_mutex_unlock(&sc_event_data_mutex);
      return true;
    }

    // Found registered event
    if (strcmp(name, sc_event_type_list[i].name) == 0) {
      *type = i;
      k_mutex_unlock(&sc_event_data_mutex);
      return true;
    }
  }
  k_mutex_unlock(&sc_event_data_mutex);

  sc_error_log(error_module, SC_ERROR_EVENT_GET_MODULE_OVERFLOW);
  return false;
}



/*
 * Register a callback for the given type. The type must have been queried using sc_event_type_get().
 * Returns true on success
 */
bool sc_event_register_cb(uint8_t type, sc_event_cb func)
{
  // TODO: use the real amount of registered events, not the allocated max
  if (type >= SC_EVENT_MAX_TYPE) {
    sc_error_log_int(error_module, SC_ERROR_EVENT_REGCB_MODULE_UNKNOWN, type);
    return false;
  }

  if (sc_event_type_list[type].cb != NULL) {
    sc_error_log_int(error_module, SC_ERROR_EVENT_REGCB_ALREADY_REGISTERED, type);
    return false;
  }

  sc_event_type_list[type].cb = func;

  return true;
}


bool sc_event_register_type_cb(const char *name, sc_event_cb func)
{
  uint8_t type;
  if (sc_event_register_type(name, &type) == false) {
    return false;
  }
  if (sc_event_register_cb(type, func) == false) {
    // TODO: event is now registered. Does it matter?
    return false;
  }
  return true;
}
