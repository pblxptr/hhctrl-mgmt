#!/bin/bash

## Todo: draft, for more advanced script use python

sdk_path=/opt/cygnos-dev/1.0.0/environment-setup-arm1176jzfshf-vfp-poky-linux-gnueabi

if [ $# -eq 0 ]
then
		echo "Too few arguments"
		echo "Usage: $0 arch (supported host, target)"
		return
fi

arch=$1

if [ $arch != "host" ] && [ $arch != "target" ]
then 
		echo "Invalid param"
		return
fi

rm -r build
mkdir build
cd build

echo "Setup environment for: $arch"

if [ $arch = "host" ]; then
		conan install .. --build=missing
		cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake -DCMAKE_BUILD_TYPE=Debug
else
	if [ -f "$sdk_path" ]; then
			source $sdk_path
			cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
	else
			echo "SDK not installed"
	fi
fi

echo "Done"
