/*
 * Copyright 2018 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Error logging and tracking
 *
 * All logged errors in the system must be unique. The error logging and
 * tracking module keeps track of the number of each error and provides the
 * timestamp of the last occurance.
 *
 * @defgroup sc_error Error logging
 * @ingroup sc_core
 * @{
 */

#ifndef SC_ERROR_H
#define SC_ERROR_H

#include "sc.h"

/**
 * @brief Representation of a single error type. Each error must be unique in the system.
 */
typedef struct sc_error_data {
  uint32_t count;
  int64_t ts;
  char *message;
} sc_error_data;

/**
 * @brief Register errors for a module.
 *
 * @param name Name of the module.
 * @param data An array of errors for the module. The error struct fields must
 * be initialized to zero except for the message that must be the name of the
 * error.
 * @param data_size Number of error elements in the data array.
 * @param module The index number of the module. This must be called when
 * logging the actual errors. This is only set if the registration succeeds.
 *
 * @retval true if successful.
 * @retval false if failed.
 */
bool sc_error_register(char *name, sc_error_data *data, uint8_t data_size, uint8_t *module);

/**
 * @brief Log an error.
 *
 * The module must be acquired first by calling sc_error_register.
 *
 * @param module Index of the module.
 * @param error Index of the error in the module's error list passed to sc_error_register.
 *
 * @retval true if successful.
 * @retval false if failed.
 */
void sc_error_log(uint8_t module, uint8_t error);

/**
 * @brief Log an error with one integer parameter.
 *
 * Same as sc_error_log but with additional integered parameter. The module must
 * be acquired first by calling sc_error_register.
 *
 * @param module Index of the module.
 * @param error Index of the error in the module's error list passed to
 * sc_error_register.
 * @param param Integer variable to be printed as a part of the error message.
 *
 * @retval true if successful.
 * @retval false if failed.
 */void sc_error_log_int(uint8_t module, uint8_t error, uint32_t param);

/**
 * @}
 */

#endif // SC_ERROR_H
