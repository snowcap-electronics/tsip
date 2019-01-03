/*
 * Copyright 2018 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sc.h"
#include <misc/printk.h>

static void event_test_cb(void);
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

int main(void)
{
  // Register error module
  uint8_t error_module;
  if (sc_error_register("MAIN", sc_error_list1, ARRAY_SIZE(sc_error_list1), &error_module) == false) {
    sc_halt();
  }

  // Register test event type and callback
  uint8_t testevent_type;
  if (sc_event_register_type("TESTMAIN", &testevent_type) == false){
    main_halt();
  }
  if (sc_event_register_cb(testevent_type, &event_test_cb) == false){
    main_halt();
  }

  uint32_t subsystems = 0;
  if (sc_init(subsystems) == false) {
    main_halt();
  }

  // Main thread will loop forever. Event thread will execute callbacks.
  while(1) {
    printk("Sending TESTMAIN from main!\n");
    if (sc_event_post(testevent_type) == false || true) {
      sc_error_log(error_module, SC_ERROR_MAIN_EVENT_POST_FAIL);
    }
    k_sleep(10000);
  }

  return 0;
}

static void event_test_cb(void)
{
  printk("TEST\n");
}

// TODO: implement sc_halt with an application specific halt function?
static void main_halt(void)
{
  // TODO: Blink big red warning light? Reset after a while?
  while(1) {};
}
