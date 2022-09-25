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
  },
  {
    \"model\" : \"temp sensor\",
    \"compatible\" : \"sysfs_ds18b20\",
    \"sysfs_path\" : \"/tmp/temp_sensor\"
  },
  {
    \"model\" : \"cpu temp sensor\",
    \"compatible\" : \"sysfs_cpu_temp_sensor\",
    \"sysfs_path\" : \"/tmp/cpu_temp\"
  }
]
" > /tmp/hhctrl-dtree.json

mkdir /tmp/temp_sensor
touch /tmp/temp_sensor/w1_slave
echo "14 02 4b 46 7f ff 0c 10 f3 : crc=f3 YES
14 02 4b 46 7f ff 0c 10 f3 t=33250
" > /tmp/temp_sensor/w1_slave


mkdir /tmp/cpu_temp
echo "40084" > /tmp/cpu_temp/temp

echo "
{
  \"dtree_file\" : \"/tmp/hhctrl-dtree.json\",
  \"entity_client_config\" : {
    \"server_address\" : \"192.168.0.115\",
    \"server_port\" : 1883,
    \"keep_alive_interval\" : 30,
    \"max_reconnect_attempts\" : 10,
    \"reconnect_delay_seconds\" : 20
  }
}
" > /tmp/app-config.json
