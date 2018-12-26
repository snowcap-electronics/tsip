/*
 * Copyright 2018 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Event Loop
 *
 * The event loop implements the asynchronous core logic of the framework. It
 * enables different drivers and tasks to run asynchronously and notify the main
 * application once there is an event to react to.
 *
 * @defgroup sc_event_loop Event Loop
 * @ingroup sc_core
 * @{
*/

#ifndef SC_EVENT_H
#define SC_EVENT_H

#include "sc.h"

/** Callback for an event */
typedef void (*sc_event_cb)(void);

/**
 * @brief Initialize the event loop.
 *
 * Must be called before initializing other subsystems.
 *
 * @retval true if successful.
 * @retval false if failed.
 */
bool sc_event_init(void);

/**
 * @brief Post an event.
 *
 * The type must be registered first with sc_event_register_type (TODO: link)
 *
 * @param type Event type.
 *
 * @retval true if successful.
 * @retval false if failed.
 */
bool sc_event_post(uint8_t type);

/**
 * @brief Register a type
 *
 * Calling this function will return an index for the type. If one did not exist
 * yet, it was registered. If it was already registered, the index of that event
 * is returned.
 *
 * @param name Name of the event. The name must be a well-known name between the
 * caller and the callee.
 * @param type Index the type is returned in this pointer if the retval is true.
 *
 * @retval true if successful.
 * @retval false if failed.
 */
bool sc_event_register_type(const char *name, uint8_t *type);

/**
 * @brief Register a callback for an event type.
 *
 * The callback will be called from the Event Loop's thread. The callback must
 * return as soon as possible to unblock handling of other events.
 *
 * The type must be registered first with sc_event_register_type (TODO: link)
 *
 * @param type Event type.
 * @param func Callback function.
 *
 * @retval true if successful.
 * @retval false if failed.
 */
bool sc_event_register_cb(uint8_t type, sc_event_cb func);

/**
 * @brief A convenience function to register and event and assign a callback.
 *
 * Internally this calls first sc_event_register_type() and then
 * sc_event_register_cb(). Event type is not returned.
 *
 * @retval true if successful.
 * @retval false if failed.
 */
bool sc_event_register_type_cb(const char *name, sc_event_cb func);

/**
 * @}
 */

#endif // SC_EVENT_H
