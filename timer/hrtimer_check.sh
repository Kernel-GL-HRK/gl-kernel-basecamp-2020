#!/bin/bash

cat /boot/config-$(uname -r) | grep CONFIG_HIGH_RES_TIMERS
sudo cat /proc/timer_list | grep .resolution

