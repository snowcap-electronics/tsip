#!/bin/bash

set -ex

PROJECTS=("test native_posix"
		  "test nucleo_f411re")

for project in "${PROJECTS[@]}"
do
	IFS=' ' read -r -a params <<< "${project}"
	app="${params[0]}"
	board="${params[1]}"
	. tsip-env.sh "${app}" "${board}"
	rm -rf build
	mkdir build
	cd build
	cmake ..
	make
	make clean
	cd ..
done
