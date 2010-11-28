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

LOSETUPED=0
$LOOPHDD set "$IMG" && LOSETUPED=1

MOUNTED=0
$MOUNTHDD mount "$IMG" && MOUNTED=1

echo -e "\\033[1;31m ***** type 'Ctrl-d/exit' to exit and umount/unset $IMG*****"
bash

[[ $MOUNTED == 1 ]] && $MOUNTHDD umount "$IMG"
[[ $LOSETUPED == 1 ]] && $LOOPHDD unset "$IMG"
