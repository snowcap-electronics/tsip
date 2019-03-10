#!/bin/bash

set -ex

PROJECTS=("test native_posix"
          "test nucleo_f411re"
          "sht3x nucleo_f411re")

export PATH=$(pwd)/zephyr/build/kconfig:$PATH
export ZEPHYR_TOOLCHAIN_VARIANT=gccarmemb
export GNUARMEMB_TOOLCHAIN_PATH=/usr

for project in "${PROJECTS[@]}"
do
    IFS=' ' read -r -a params <<< "${project}"
    app="${params[0]}"
    board="${params[1]}"
    if [ "${1}" == "coverity" ]
    then
        if [ "$board" == "native_posix" ]
        then
            echo "Skipping non-arm board for Coverity runs: ${board}"
            continue
        fi
    fi
    . tsip-env.sh "${app}" "${board}"
    rm -rf build
    mkdir build
    cd build
    cmake ..
    make
    make clean
    cd ..
done
