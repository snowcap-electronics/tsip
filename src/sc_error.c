/*
 * Copyright 2018 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sc_error.h"
#include <misc/printk.h>

#ifndef SC_ERROR_MODULE_MAX
#define SC_ERROR_MODULE_MAX 8
#endif

typedef struct sc_error_module_data {
  const char *name;
  sc_error_data *data;
  uint8_t data_size;
} sc_error_module_data;

sc_error_module_data sc_error_module_data_list[SC_ERROR_MODULE_MAX];
static void sc_error_log_ext(uint8_t module, uint8_t error, bool has_param, uint32_t param);

K_MUTEX_DEFINE(sc_error_module_data_mutex);

bool sc_error_register(char *name, sc_error_data *error_data, uint8_t data_size, uint8_t *module)
{
  if (name == NULL || name[0] == '\0' || error_data == NULL || data_size == 0) {
    // TODO: no asserts in error module, print error if possible, optionally with the module name
    return false;
  }

  k_mutex_lock(&sc_error_module_data_mutex, K_FOREVER);
  for (uint8_t i = 0; i < SC_ERROR_MODULE_MAX; ++i) {
    sc_error_module_data *data = &sc_error_module_data_list[i];
    if (data->name == NULL) {
      data->name = name;
      data->data = error_data;
      data->data_size = data_size;
      *module = i;
      k_mutex_unlock(&sc_error_module_data_mutex);
      return true;
    }
  }
  k_mutex_unlock(&sc_error_module_data_mutex);

  // TODO: no asserts in error module, print error if possible, optionally with the module name
  return false;
}



static void sc_error_log_ext(uint8_t module_index, uint8_t error, bool has_param, uint32_t param)
{
  sc_error_module_data *module = &sc_error_module_data_list[module_index];
  // TODO: should use the real max module_index, not the define
  if (module_index < SC_ERROR_MODULE_MAX &&
      module->name != NULL &&
      error < module->data_size) {

    module->data[error].count++;
    module->data[error].ts = k_uptime_get();
    // TODO: should not use kprint but uart etc.
    // TODO: kprint (with optional param)
    printk("error: %lld, %s, %d, %s\n", module->data[error].ts, module->name, error, module->data[error].message);
    // TODO: Optionally assert?
  } else {
    // TODO: print error about error
    printk("sc_error_log_ext failed\n");
    // TODO: Optionally assert?
  }
}



void sc_error_log(uint8_t module, uint8_t error)
{
  sc_error_log_ext(module, error, false, 0);
}



void sc_error_log_int(uint8_t module, uint8_t error, uint32_t param)
{
  sc_error_log_ext(module, error, true, param);
}
