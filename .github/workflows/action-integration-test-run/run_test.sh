#!/bin/bash

if [ $# -lt 3 ]
then
		echo "Too few arguments"
		echo "Usage: $0 image_name artifacts_dir target [ target_args... ] "
		echo "Example: $0 imag_name artifacts_dir target_name --opt1 opt1_val --opt2 opt2_val"
		exit
fi

image=$1
artifacts_local_dir=$2
artifacts_remote_dir=/home/integration_tests_binaries
target=$3

arg_idx=3
target_command=$artifacts_remote_dir/$target
args=("$@")
until [ $arg_idx -eq $# ]
do
  name_idx=$arg_idx
  val_idx=arg_idx+1
  target_command="$target_command ${args[name_idx]} ${args[val_idx]}"

  ((arg_idx=arg_idx+2))
done

echo "Docker image: $image"
echo "Artifacts local dir: $artifacts_local_dir"
echo "Artifacts remote dir: " $artifacts_remote_dir
echo "Target: $target"
echo "Target command: $target_command"

### Run docker


docker run -v $artifacts_local_dir:$artifacts_remote_dir \
  $image:$target \
  /bin/bash -c "
    ls -la $artifacts_remote_dir && \
    chmod +x $artifacts_remote_dir/$target && \
    $target_command && \
    exit $?
  "
