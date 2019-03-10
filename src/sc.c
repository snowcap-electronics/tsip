/*
 * Copyright 2018 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sc.h"
#include <shell/shell.h>

#ifdef CONFIG_SHELL
static int sc_shell_cmd_ping(const struct shell *shell, size_t argc, char **argv);
static int sc_shell_cmd_version(const struct shell *shell, size_t argc, char **argv);
#endif

bool sc_init(uint32_t subsystems)
{
  // TODO: initialize the subsystems properly
  (void) subsystems;

  /* The Event loop must be initialized before any other subsystem */
  if (sc_event_init() == false) {
    return false;
  }

  return true;
}


// TODO: implement sc_halt with an application specific halt function?
// Init to register a callback to call and if NULL, do a halt here?
void sc_halt(void)
{
  // TODO: loop for ever on debug builds, 10 ms on release builds?
  while(1) {};
}

#ifdef CONFIG_SHELL
static int sc_shell_cmd_ping(const struct shell *shell, size_t argc, char **argv)
{
  (void)argc;
  (void)argv;

  shell_print(shell, "pong");

  return 0;
}

SHELL_CMD_REGISTER(ping, NULL, "Ping pong", sc_shell_cmd_ping);

static int sc_shell_cmd_version(const struct shell *shell, size_t argc, char **argv)
{
  (void)argc;
  (void)argv;

  // TODO: use git hash?
  shell_print(shell, "0.1");

  return 0;
}
SHELL_CMD_REGISTER(version, NULL, "Shopw version number", sc_shell_cmd_version);
#endif