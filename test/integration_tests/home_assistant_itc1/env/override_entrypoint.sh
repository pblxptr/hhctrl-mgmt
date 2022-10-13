#!/bin/ash
set -e

echo "Rehash password file"
mosquitto_passwd -U /etc/mosquitto/password_file

# Set permissions
user="$(id -u)"
if [ "$user" = '0' ]; then
        [ -d "/mosquitto" ] && chown -R mosquitto:mosquitto /mosquitto || true
fi

exec "$@"
