#!/bin/bash
#
# mounthdd.sh
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

if [[ $# -lt 2 ]]
then
	echo "usage: $0 mount|umount hdd.img [only_boot]"
	exit 1
fi

ACTION=$1
IMG="$2"
ONLYBOOT="$3"
		
p1d="$IMG.p1"
p2d="$IMG.p2"

case $1 in
	mount)
		echo "Find loop device for $IMG..."
		loop="$(sudo losetup -j "$IMG" | sed 's/:.*$//')"
		if [[ -z "$loop" ]]
		then
			echo "Unable to found any loop device linked to $IMG..."
			exit 2
		fi
		echo "Loop device is $loop"
		loop_basename="$(basename "$loop")"
		p1="/dev/mapper/${loop_basename}p1"
		p2="/dev/mapper/${loop_basename}p2"

		echo "Mouting $p1 to $p1d..."
		mkdir "$p1d" && sudo mount "$p1" "$p1d"

		if [[ -z "$ONLYBOOT" ]]
		then
			echo "Mouting $p2 to $p2d..."
			mkdir "$p2d" && sudo mount "$p2" "$p2d"
		fi
		;;
	umount)
		if [[ -d "$p2d" ]]
		then
			echo "Unmounting $p2d..."
			sudo umount "$p2d" && rmdir "$p2d"
		fi
		echo "Unmounting $p1d..."
		sudo umount "$p1d" && rmdir "$p1d"
		;;
	*)
		echo "Unknown action"
		;;
esac

