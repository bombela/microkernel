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

cat >gdbexec <<EOF
	sym "$KERNEL"
EOF

if [[ -n "$HDDIMG" ]]
then
	cat >>gdbexec <<EOF
	target remote | exec kvm -no-kvm -gdb stdio -hda "$HDDIMG"
EOF
else
	cat >>gdbexec <<EOF
	target remote | exec kvm -no-kvm -gdb stdio -kernel "$KERNEL"
EOF
fi

cat >>gdbexec <<EOF
	break kernel_main
	continue
	layout split
	focus cmd
	break task.cpp:100
	continue
EOF

gdb -tui -x gdbexec
