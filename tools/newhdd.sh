#!/bin/bash
#
# newhdd.sh
# Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
#
# grub-firmware-qemu

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
echo "Loop device is $loop" 

echo "Creating DOS master boot record..."
sudo parted "$loop" mklabel msdos

echo "Creating following partition scheme:"
echo "  primary0 ext2, 24MB"
echo "  primary1 fat32, $SIZE - 24MB"
sudo fdisk "$loop"<<EOF
n
p
1

+16M
a
1
n
p
2


t
2
c
w
EOF

# sudo sfdisk -q --no-reread "$loop" -uM <<EOF
# 0,1,0
# ,16,83,*
# ,,0C,-
# EOF

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
	grubFolder="${p1d}/grub"

	echo "Copying grub files..."
	sudo cp -R /usr/lib/grub/i386-pc "$grubFolder"

	echo "Generating grub files..."

	sudo -s <<EOF 
cat > "${grubFolder}/device.map" <<EOF2
(hd0)   $loop
EOF2
EOF
	sudo -s <<EOF
cat > "${grubFolder}/grub.cfg" <<EOF2
set timeout=0

menuentry 'kern0x' --class microkernel --class os {
	insmod part_msdos
	insmod ext2
	insmod multiboot
	set root='(hd0, 1)'
	multiboot /kern0x my pretty kernel
}
EOF2
EOF
	
	sudo grub-mkimage -O i386-pc --output="${grubFolder}/core.img" \
		--prefix="/grub" ext2 biosdisk part_msdos
	
	echo "Installing grub..."
	sudo grub-setup --skip-fs-probe --device-map="${grubFolder}/device.map" \
		--directory="$grubFolder" --root-device='(hd0,1)' '(hd0)'
else
	echo "Mount error... oops..."
fi
$MOUNTHDD umount "$IMG"


echo "Unload partitions by the kernel..."
sudo kpartx -v -d "$loop"

echo "Loop detach $loop..."
sudo losetup -d "$loop"
