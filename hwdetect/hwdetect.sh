#!/usr/bin/env bash

readonly EXIT_OK=0

print_usb_to_ttl_convertors() {
  printf "USB TO TTL CONVERTORS\n"
  printf "=====================\n"
  printf "%-15s %-10s %-s\n" "DEVICE" "MAJ:MIN" "MODEL"

  for dev in $(find /dev -name "ttyUSB*" | sort); do
    local major=$(udevadm info ${dev} | grep "MAJOR" | cut -d "=" -f2)
    local minor=$(udevadm info ${dev} | grep "MINOR" | cut -d "=" -f2)
    local model=$(udevadm info ${dev} | grep "ID_MODEL_FROM_DATABASE" | cut -d "=" -f2)

    printf "%-15s %-10s %-s\n" "${dev}" "${major}:${minor}" "${model}"
  done
}

print_block_devices() {
  printf "BLOCK DEVICES\n"
  printf "=============\n"
  printf "%-15s %-10s %-10s %-s\n" "NAME" "MAJ:MIN" "SIZE" "MOUNTPOINT"

  lsblk -dn | awk '{ printf("%-15s %-10s %-10s %-s\n", $1, $2, $4, $7) }'
}

print_i2c_devices() {
  printf "I2C DEVICES\n"
  printf "===========\n"
  printf "%-10s %s\n" "ADAPTER" "ADDRESS"

  for adapter in $(i2cdetect -l | grep -oP "i2c-\K(\d*)" | sort); do
    for address in $(i2cdetect -y ${adapter} | grep -oP "\s\K(\d{2})\s" | sort); do
      printf "%-10s 0x%d\n" "i2c-${adapter}" "${address}"
    done
  done
}

main() {
  print_block_devices

  echo

  print_i2c_devices

  echo

  print_usb_to_ttl_convertors

  return ${EXIT_OK}
}

main "$@"
