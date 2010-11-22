#!/bin/bash
#
# newhdd.sh
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
#

SIZE=2G

if [[ $# -ne 1 ]]
then
	echo "usage: $0 hdd.img"
	exit 1
fi

IMG="$1"
BASE_DIR="$(dirname "$0")"
MOUNTHDD="$BASE_DIR/mounthdd.sh"

if [[ -e "$IMG" ]]
then
	echo "File exist: $IMG"
	echo "Erase it or choose another name."
	exit 2
fi

echo "Creating a $SIZE image to $IMG..."
dd if=/dev/zero of="$IMG" bs=1 seek=$SIZE count=0
du -h --apparent-size "$IMG"

echo "Loop setup image..."
loop="$(sudo losetup -v -f "$IMG" | sed 's/^Loop device is //')"
echo "Lopp device is $loop" 

echo "Creating DOS master boot record..."
sudo parted "$loop" mklabel msdos

echo "Creating following partition scheme:"
echo "  primary0 ext2, 24MB"
echo "  primary1 fat32, $SIZE - 24MB"
sudo sfdisk -q --no-reread "$loop" -uM <<EOF
0,16,83,*
,,0C,-
EOF

echo "Load partitions by the kernel..."
sudo kpartx -v -a "$loop"

loop_basename="$(basename "$loop")"
p1="/dev/mapper/${loop_basename}p1"
p2="/dev/mapper/${loop_basename}p2"

echo "Create file systems..."
sudo mke2fs -t ext2 -T small -m 0 -v "$p1"
sudo tune2fs -c 0 -i 0 "$p1"
sudo mkdosfs -F 32 -v "$p2"

echo "Mounting filesystems..."
if $MOUNTHDD mount "$IMG"
then
	p1d="$IMG.p1"
	p2d="$IMG.p2"

	echo "Copying grub files..."
	sudo cp -R /usr/lib/grub/i386-pc/ "$p1d/grub"

	echo "Installing grub..."
	sudo grub-setup "$p1"
else
	echo "Mount error... oops..."
fi
$MOUNTHDD umount "$IMG"

echo "Unload partitions by the kernel..."
sudo kpartx -v -d "$loop"

echo "Loop detach $loop..."
sudo losetup -d "$loop"
