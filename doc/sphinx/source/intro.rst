Intro
=====

The goal of the Tsip is to enable writing applications for multiple use cases where all the functionality is run in parallel. When something happens, e.g. data from a sensor is ready, an event is sent to the main application and the main application can then fetch the data from the sensor driver when it has time.

Examples of applications that benefit from parallelism:

IMU where all the 9 axis must be read when ready and heavy math operations can be calculated in a separate thread.
A remote controlled vehicle where the motors are controlled at the same time the sensors are read.
TODO: Briefly describe EventLoop.

TODO: Briefly describe Error handling/reporting.  