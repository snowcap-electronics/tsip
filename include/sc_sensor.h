/*
 * Copyright 2019 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Sensor framework for Zephyr's sensor API
 *
 * Support multiple sensors using Zephyr's sensor API. Provide events to the
 * main application using the event thread so that the main application doesn't
 * need to care about thread safety in its implementation.
 *
 * @{
 */

#ifndef SC_SENSOR_H
#define SC_SENSOR_H

#ifdef CONFIG_SENSOR

#include "sc.h"
#include <sensor.h>

/**
 * @brief Initialize the sensor subsystem.
 *
 * @retval true if successful.
 * @retval false if failed.
 */
bool sc_sensor_init(void);

/**
 * @brief Add a sensor to be polled with the given interval
 *
 * @param device_name Name of the device for device_get_binding().
 * @param channels Zephyr sensor channels to be polled.
 * @param channel_count Number of channels.
 * @param interval_ms Poll interval in milliseconds.
 * @param event_name Event to send when a new data is available.
 * @param index The index of the sensor if succesful.
  *
 * @retval true if successful.
 * @retval false if failed.
 */

bool sc_sensor_add(const char *device_name,
                   enum sensor_channel *channels,
                   uint8_t channel_count,
                   uint32_t interval_ms,
                   const char *event_name,
                   uint8_t *index);

/**
 * @brief Read the sensor value after receiving the event indicating a new value
 * is ready.
 *
 * @param index The index number of the sensor received with the sc_sensor_add
 * function.
 * @param value The returned values of the latest sensor read. The array
 * contains channel_count values listed with the sc_sensor_add function.
 *
 * @retval true if successful.
 * @retval false if failed.
 */
bool sc_sensor_get_value(uint8_t index, struct sensor_value *values);


/**
 * @brief Helper function for printing a sensor value to a buffer
 *
 * @param buf A pointer to the buffer to which the printing is done.
 * @param len Size of buffer in bytes.
 * @param val A pointer to a sensor_value struct holding the value
 *            to be printed.
 *
 * @return The number of characters printed to the buffer.
 *
 * Copied from:
 * https://github.com/zephyrproject-rtos/zephyr/blob/master/samples/sensor/bmi160/src/bmi160.c
 */
int sc_sensor_value_snprintf(char *buf, size_t len,
                             const struct sensor_value *val);
/**
 * @}
 */

#endif // CONFIG_SENSOR
#endif // SC_SENSOR_H
