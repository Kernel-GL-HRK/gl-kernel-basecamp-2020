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

main() {
  print_usb_to_ttl_convertors

  return ${EXIT_OK}
}

main "$@"
