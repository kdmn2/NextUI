#!/bin/sh

SDCARD_PATH="/mnt/SDCARD"
USERDATA_PATH="$SDCARD_PATH/.userdata"


cd $(dirname "$0")


./bluetooth.elf > $USERDATA_PATH/tg5040/logs/bluetooth.log 2>&1
