.. image:: https://readthedocs.org/projects/tsip/badge/?version=latest
    :target: https://tsip.readthedocs.org/
.. image:: https://travis-ci.org/snowcap-electronics/tsip.svg?branch=master
    :target: https://travis-ci.org/snowcap-electronics/tsip
.. image:: https://scan.coverity.com/projects/17592/badge.svg
    :target: https://scan.coverity.com/projects/snowcap-electronics-tsip

Snowcap Tsip
############

Please see the project documentation in `Read the docs <http://tsip.readthedocs.io/>`_.

Overview
********
Zephyr based RTOS multiapp framework

Build
*****

Source the tsip-env.sh to get appropriate environment variables for the project. It will source the zephyr-env.sh as well.

The tsip-env.sh takes the application name and the board as parameters. The application name is used to compile the app using ``src/main-<app_name>.c`` source file.
E.g.:

.. code-block:: sh
$ . tsip-env.sh test native_posix
