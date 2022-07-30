#!/bin/bash

mkdir -p /tmp/misc/hatch2sr
touch /tmp/misc/hatch2sr/status
echo "closed" > /tmp/misc/hatch2sr/status

mkdir -p /tmp/leds/red
mkdir -p /tmp/leds/green
mkdir -p /tmp/leds/blue

echo "
[
  {
    \"model\" : \"hatch\",
    \"compatible\" : \"sysfs_hatch2sr\",
    \"sysfs_path\" : \"/tmp/misc/hatch2sr\"
  },
  {
    \"model\" : \"rgb_led\",
    \"compatible\" : \"sysfs_rgbled_indicator\",
    \"leds\" : [
      {
        \"model\"      : \"led\",
        \"compatible\" : \"sysfs_led\",
        \"sysfs_path\" : \"/tmp/leds/red\",
        \"color\" : \"red\"
      },
      {
        \"model\"      : \"led\",
        \"compatible\" : \"sysfs_led\",
        \"sysfs_path\" : \"/tmp/leds/green\",
        \"color\" : \"green\"
      },
      {
        \"model\"      : \"led\",
        \"compatible\" : \"sysfs_led\",
        \"sysfs_path\" : \"/tmp/leds/blue\",
        \"color\" : \"blue\"
      }
    ]
  }
]
" > /tmp/dtree.json

mkdir /tmp/temp_sensor
touch /tmp/temp_sensor/w1_slave
echo "33312" > /tmp/temp_sensor/w1_slave