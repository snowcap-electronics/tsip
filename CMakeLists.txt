cmake_minimum_required(VERSION 3.8.2)
include_directories(include)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

target_sources(app PRIVATE src/main-$ENV{TSIP_APP}.c)
target_sources(app PRIVATE src/sc.c)
target_sources(app PRIVATE src/sc_error.c)
target_sources(app PRIVATE src/sc_event.c)
target_sources(app PRIVATE src/sc_sensor.c)

