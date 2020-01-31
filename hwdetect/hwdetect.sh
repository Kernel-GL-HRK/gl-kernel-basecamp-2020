#!/usr/bin/env bash

readonly EXIT_OK=0

get_device_major() {
  local readonly device=$1
  local readonly major=$(udevadm info ${device} | grep "MAJOR" | cut -d "=" -f2)

  echo "${major}"
}

get_device_minor() {
  local readonly device=$1
  local readonly minor=$(udevadm info ${device} | grep "MINOR" | cut -d "=" -f2)

  echo "${minor}"
}

get_block_devices() {
  local devices=$(lsblk -dn | cut -d " " -f1 | sed -e 's#^#/dev/#' | sort)

  echo "${devices}"
}

get_i2c_devices() {
  local devices=()

  for adapter in $(i2cdetect -l | grep -oP "i2c-\K(\d*)" | sort); do
    for address in $(i2cdetect -y ${adapter} | grep -oP "\s\K(\d{2})\s" | sort); do
      devices+=("/dev/i2c-${adapter}:0x${address}")
    done
  done

  echo "${devices[@]}"
}

get_i2c_device_adapter() {
  local readonly device=$1
  local readonly adapter=$(echo ${device} | cut -d ":" -f1)

  echo "${adapter}"
}

get_i2c_device_address() {
  local readonly device=$1
  local readonly address=$(echo ${device} | cut -d ":" -f2)

  echo "${address}"
}

get_usb_to_ttl_convertors() {
  local readonly devices=$(find /dev -name "ttyUSB*" | sort)

  echo "${devices}"
}

get_usb_to_ttl_convertor_model() {
  local readonly device=$1
  local readonly model=$(
    udevadm info ${device} | grep "ID_MODEL_FROM_DATABASE" | cut -d "=" -f2
  )

  echo "${model}"
}

print_block_devices() {
  local readonly devices=($1)

  if [[ -z "${devices}" ]]; then
    return
  fi

  printf "BLOCK DEVICES\n"
  printf "=============\n"
  printf "%-15s %-10s\n" "DEVICE" "MAJ:MIN"

  for device in "${devices[@]}"; do
    local major=$(get_device_major ${device})
    local minor=$(get_device_minor ${device})

    printf "%-15s %-10s\n" "${device}" "${major}:${minor}"
  done

  echo
}

print_i2c_devices() {
  local readonly devices=($1)

  if [[ -z "${devices}" ]]; then
    return
  fi

  printf "I2C DEVICES\n"
  printf "===========\n"
  printf "%-15s %-10s %s\n" "DEVICE" "MAJ:MIN" "ADDRESS"

  for device in "${devices[@]}"; do
    local adapter=$(get_i2c_device_adapter ${device})
    local major=$(get_device_major ${adapter})
    local minor=$(get_device_minor ${adapter})
    local address=$(get_i2c_device_address ${device})

    printf "%-15s %-10s %s\n" "${adapter}" "${major}:${minor}" "${address}"
  done

  echo
}

print_usb_to_ttl_convertors() {
  local readonly devices=($1)

  if [[ -z "${devices}" ]]; then
    return
  fi

  printf "USB TO TTL CONVERTORS\n"
  printf "=====================\n"
  printf "%-15s %-10s %-s\n" "DEVICE" "MAJ:MIN" "MODEL"

  for device in "${devices[@]}"; do
    local major=$(get_device_major ${device})
    local minor=$(get_device_minor ${device})
    local model=$(get_usb_to_ttl_convertor_model ${device})

    printf "%-15s %-10s %-s\n" "${device}" "${major}:${minor}" "${model}"
  done

  echo
}

main() {
  print_block_devices "$(get_block_devices)"
  print_i2c_devices "$(get_i2c_devices)"
  print_usb_to_ttl_convertors "$(get_usb_to_ttl_convertors)"

  return ${EXIT_OK}
}

main "$@"
