#!/bin/bash
#
# cpykern.sh
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

if [[ $# -ne 2 ]]
then
	echo "usage: $0 kernel img"
	exit 1
fi

KERNEL="$1"
IMG="$2"

p1d="$IMG.p1"
KERNEL_DST="$p1d/kern0x"

BASE_DIR="$(dirname "$0")"
LOOPHDD="$BASE_DIR/loophdd.sh"
MOUNTHDD="$BASE_DIR/mounthdd.sh"

MOUNTED=0
$MOUNTHDD mount "$IMG" only_boot && MOUNTED=1

if ! [[ -d "$p1d" ]]
then
	echo "Mount error!"
	exit 1
fi

echo "Copying kernel $KERNEL to $KERNEL_DST..."
sudo cp "$KERNEL" "$KERNEL_DST"

[[ $MOUNTED == 1 ]] && $MOUNTHDD umount "$IMG"
