#!/bin/bash

# Split 15T and 2 T
echo "Create two files, with size 15T and 2T"
dd if=/dev/zero of=ext4_15T bs=4k count=1 seek=$((256*1024*1024*15))
dd if=/dev/zero of=ext4_2T bs=4k count=1 seek=$((256*1024*1024*2))

# associate with loop device
echo "associate with loop device"
losetup /dev/loop0 ext4_15T
losetup /dev/loop1 ext4_2T

# lvm 17T files
echo "lvm"
echo "vim /etc/lvm/lvm.conf, change filter to accept all device"
sleep 2
vim /etc/lvm/lvm.conf
pvcreate /dev/loop0 /dev/loop1
vgcreate vg_meta /dev/loop0 /dev/loop1
lvcreate -l 100%FREE -n lv1 vg_meta
echo "mkfs.ext4"
mkfs.ext4 -O 64bit,meta_bg /dev/vg_meta/lv1

echo "Test data is ready!"
