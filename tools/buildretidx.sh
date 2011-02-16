#!/bin/bash
#
# traceret.sh
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#

cat <<EOF
#include <kernel/types.h>

uint8_t* ret_addr[] = {
EOF

sed -n 's/^ *\([0-9a-z]\+\):.*c3.*\<ret\>/\1/p' "$1" | while read addr
do
	echo "(uint8_t*)0x$addr, "
done
echo -e "0x0\n};"
