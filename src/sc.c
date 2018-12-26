/*
 * Copyright 2018 Tuomas Kulve, <tuomas@kulve.fi>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sc.h"
#include <shell/shell.h>

static int sc_shell_cmd_ping(int argc, char *argv[]);

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

static int sc_shell_cmd_ping(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  printk("pong\n");

  return 0;
}
#define SHELL_CMD_PING "ping"
SHELL_REGISTER_COMMAND(SHELL_CMD_PING, sc_shell_cmd_ping, "Ping pong");

static int sc_shell_cmd_version(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  // TODO: use git hash?
  printk("0.1\n");

  return 0;
}
#define SC_SHELL_CMD_VERSION "version"
SHELL_REGISTER_COMMAND(SC_SHELL_CMD_VERSION, sc_shell_cmd_version, "Show version number");


