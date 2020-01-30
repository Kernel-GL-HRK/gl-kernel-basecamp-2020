#!/usr/bin/env bash

readonly EXIT_OK=0

print_usb_to_ttl_convertors() {
  printf "USB TO TTL CONVERTORS\n\n"
  printf "%-15s %-10s %-s\n" "DEVICE" "MAJ:MIN" "MODEL"

  for dev in $(find /dev -name "ttyUSB*" | sort); do
    local major=$(udevadm info ${dev} | grep "MAJOR" | cut -d "=" -f2)
    local minor=$(udevadm info ${dev} | grep "MINOR" | cut -d "=" -f2)
    local model=$(udevadm info ${dev} | grep "ID_MODEL_FROM_DATABASE" | cut -d "=" -f2)

    printf "%-15s %-10s %-s\n" "${dev}" "${major}:${minor}" "${model}"
  done
}

print_block_devices() {
  printf "BLOCK DEVICES\n\n"

  lsblk -dn | awk '
  BEGIN {
    printf("%-15s %-10s %-10s %-s\n", "NAME", "MAJ:MIN", "SIZE", "MOUNTPOINT")
  } {
    printf("%-15s %-10s %-10s %-s\n", $1, $2, $4, $7)
  }
  '
}

main() {
  print_usb_to_ttl_convertors

  echo

  print_block_devices

  return ${EXIT_OK}
}

main "$@"
