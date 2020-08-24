#!/bin/sh

# Create a 10M sparse file, see: https://en.wikipedia.org/wiki/Sparse_file
filename="../data/fst-newbie"
dd of=$filename bs=10M seek=1 count=0

# Associate loop devices with regular files or block devices
# see: https://linux.die.net/man/8/losetup
# alos see: https://gitmemory.com/issue/marcone/teslausb/306/539802870
next_dev_loop=$(losetup -f | cut -f3)
echo "next_dev_loop=$next_dev_loop"
losetup $next_dev_loop "$filename"

# Format it as ext4
mkfs.ext4 $next_dev_loop

echo "To undo losetup:"
echo "losetup -d $next_dev_loop"
