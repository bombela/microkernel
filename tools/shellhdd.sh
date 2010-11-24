#!/bin/bash

if [[ $# -ne 1 ]]
then
	echo "usage: $0 hdd.img"
	exit 1
fi

IMG="$1"
BASE_DIR="$(dirname "$0")"
MOUNTHDD="$BASE_DIR/mounthdd.sh"
LOOPHDD="$BASE_DIR/loophdd.sh"

$LOOPHDD set "$IMG"
$MOUNTHDD mount "$IMG"
echo -e "\\033[1;31m ***** type 'Ctrl-d/exit' to exit and umount/unset $IMG*****"
bash
$MOUNTHDD umount "$IMG"
$LOOPHDD unset "$IMG"
