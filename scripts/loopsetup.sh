#!/bin/bash
filename=$1

next_dev_loop=$(losetup -f | cut -f3)
echo "next_dev_loop=$next_dev_loop"
losetup $next_dev_loop "$filename"

echo "To undo losetup:"
echo "losetup -d $next_dev_loop"
