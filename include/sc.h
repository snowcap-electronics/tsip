/*
 * Copyright 2018 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Snowcap framework common functions
 */

#ifndef SC_H
#define SC_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* Snowcap framework includes */
#include "sc_event.h"
#include "sc_error.h"
#include "sc_sensor.h"

#include <zephyr.h>

typedef enum SC_THREAD_PRIORITY {
  SC_THREAD_PRIORITY_HIGH = 3,
  SC_THREAD_PRIORITY_MEDIUM = 5,
  SC_THREAD_PRIORITY_LOW = 7,
} SC_THREAD_PRIORITY;

bool sc_init(uint32_t subsystems);
void sc_halt(void);

#endif // SC_H
