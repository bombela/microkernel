#!/bin/bash
#
# loophdd.sh
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

if [[ $# -ne 2 ]]
then
	echo "usage: $0 set|unset hdd.img"
	exit 1
fi

ACTION=$1
IMG="$2"

case $1 in
	set)
		echo "Loop setup image $IMG..."
		loop="$(sudo losetup -v -f "$IMG" | sed 's/^Loop device is //')"
		echo "Loop device is $loop" 

		echo "Load partitions by the kernel..."
		sudo kpartx -v -a "$loop"
		;;
	unset)
		echo "Find loop device for $IMG..."
		loop="$(sudo losetup -j "$IMG" | sed 's/:.*$//')"
		if [[ -z "$loop" ]]
		then
			echo "Unable to found any loop device linked to $IMG..."
			exit 2
		fi
		echo "Loop device is $loop" 

		echo "Unload partitions by the kernel..."
		sudo kpartx -v -d "$loop"

		echo "Loop detach $loop..."
		sudo losetup -d "$loop"
		;;
	*)
		echo "Unknown action"
		;;
esac
