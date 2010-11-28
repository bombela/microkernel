#!/bin/bash
#
# gdb.sh
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

if [[ $# -lt 1 ]]
then
	echo "usage: $0 kernel [hddimg]"
	exit 1
fi

KERNEL="$1"
HDDIMG="$2"

if [[ -n "$HDDIMG" ]]
then
	gdb -s "$KERNEL"
else
	cat >gdbexec <<EOF
	sym "$KERNEL"
	target remote | exec kvm -gdb stdio -kernel "$KERNEL"
	break *0x201000
	break kernel_main
	continue
	layout split
EOF
	gdb -tui -x gdbexec
fi
