#!/usr/bin/env bash

################################################################################

# A solution for hardware detector (hwdetect)
# https://github.com/Kernel-GL-HRK/gl-kernel-basecamp-2020/commit/5418201
# Author: Eduard Malokhvii <malokhvii.ee@gmail.com>

################################################################################

readonly USAGE=$(cat <<EOF
NAME

  $0 - Hardware detector

SYNOPSIS

  $0 [OPTION]...

DESCRIPTION

  A bash script that detects block devices, I2C devices, USB to TTL convertors.

  -l  Display hardware listing
  -w  Watch for hardware changes
  -d  Delay between hardware detections
  -h  Display this help and exit

AUTHOR

  Written by Eduard Malokhvii <malokhvii.ee@gmail.com>.
EOF
)

usage() {
  printf "${USAGE}\n\n"
}

################################################################################

# Exit codes

readonly EXIT_OK=0
readonly EXIT_INVAL=1

################################################################################

error() {
  printf "$@\n" >&2
}

################################################################################

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

################################################################################

get_block_devices() {
  local devices=$(lsblk -dn | cut -d " " -f1 | sed -e 's#^#/dev/#' | sort)

  echo "${devices}"
}

################################################################################

get_i2c_devices() {
  local devices=()

  for adapter in $(i2cdetect -l | grep -oP "i2c-\K(\d*)" | sort); do
    for address in $(i2cdetect -y ${adapter} | grep -oP "\s\K(\d{2})\s" | \
      sort); do
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

################################################################################

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

################################################################################

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

print_hardware_listing() {
  print_block_devices "$(get_block_devices)"
  print_i2c_devices "$(get_i2c_devices)"
  print_usb_to_ttl_convertors "$(get_usb_to_ttl_convertors)"
}

################################################################################

readonly DEFAULT_HARDWARE_DETECTION_DELAY=10
readonly HARDWARE_BEFORE_SNAPSHOT="/tmp/hwdetect-before.snapshot"
readonly HARDWARE_AFTER_SNAPSHOT="/tmp/hwdtect-after.snapshot"

write_devices_to_hardware_snapshot() {
  local readonly snapshot=$1
  local readonly label=$2
  local readonly devices=($3)

  for device in "${devices[@]}"; do
    printf "%s#%s\n" "${label}" "${device}" >> ${snapshot}
  done
}

create_hardware_snapshot() {
  local readonly snapshot=$1

  printf "timestamp %s\n" "$(date +'%Y%m%d-%H:%M:%S.%N')" > ${snapshot}

  write_devices_to_hardware_snapshot ${snapshot} "blk" "$(get_block_devices)"
  write_devices_to_hardware_snapshot ${snapshot} "i2c" "$(get_i2c_devices)"
  write_devices_to_hardware_snapshot ${snapshot} "ttyUSB" \
    "$(get_usb_to_ttl_convertors)"
}

watch_hardware_changes() {
  local readonly hardware_detection_delay=$1

  printf "HARDWARE CHANGES\n"
  printf "================\n"
  printf "%-28s %-20s %-10s %s\n" "TIMESTAMP" "DEVICE" "TYPE" "STATUS"

  while true; do
    create_hardware_snapshot ${HARDWARE_BEFORE_SNAPSHOT}
    sleep ${hardware_detection_delay}
    create_hardware_snapshot ${HARDWARE_AFTER_SNAPSHOT}

    local hardware_diff=$(
      diff \
      <(cat ${HARDWARE_BEFORE_SNAPSHOT} | tail -n +2) \
      <(cat ${HARDWARE_AFTER_SNAPSHOT} | tail -n +2) \
      | tail -n +2
    )

    if [[ -z "${hardware_diff}" ]]; then
      continue
    fi

    local readonly timestamp=$(date +"%Y%m%d-%H:%M:%S.%N")

    local readonly connected_hardware=(
      $(echo "${hardware_diff}" | grep ">" | cut -d " " -f2)
    )
    local readonly disconnected_hardware=(
      $(echo "${hardware_diff}" | grep "<" | cut -d " " -f2)
    )

    for hardware in "${connected_hardware[@]}"; do
      local device=$(echo ${hardware} | cut -d "#" -f2)
      local type=$(echo ${hardware} | cut -d "#" -f1)

      printf "%-28s %-20s %-10s %s\n" "${timestamp}" "${device}" "${type}" \
        "CONNECTED"
    done

    for hardware in "${disconnected_hardware[@]}"; do
      local device=$(echo ${hardware} | cut -d "#" -f2)
      local type=$(echo ${hardware} | cut -d "#" -f1)

      printf "%-28s %-20s %-10s %s\n" "${timestamp}" "${device}" "${type}" \
        "DISCONNECTED"
    done
  done
}

################################################################################

readonly OPTIONS="lwd:h"
readonly OPTERR=0

main() {
  local show_hardware_listing=false
  local watch_hardware_changes=false

  local hardware_detection_delay=${DEFAULT_HARDWARE_DETECTION_DELAY}

  while getopts "${OPTIONS}" option; do
    case ${option} in
      l)
        show_hardware_listing=true
        ;;
      w)
        watch_hardware_changes=true
        ;;
      d)
        hardware_detection_delay=${OPTARG}
        ;;
      h | ?)
        usage
        return ${EXIT_OK}
        ;;
    esac
  done
  shift $((OPTIND -1))

  if ! [[ ${hardware_detection_delay} -gt 0 ]]; then
    error "Wrong hardware detection delay!"
    return ${EXIT_INVAL};
  fi

  if [[ ${show_hardware_listing} == true ]]; then
    print_hardware_listing
  fi

  if [[ ${watch_hardware_changes} == true ]]; then
    watch_hardware_changes ${hardware_detection_delay}
  fi

  return ${EXIT_OK}
}

main "$@"
