#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "filesystem.h"

uint32_t div_ceil(uint32_t dividen, uint32_t divisor)
{
    if (dividen == 0) {
        return 0;
    }

    return (dividen - 1) / divisor + 1;
}

/*
* Read the super block, skip the front 1024 bytes
*/
int SuperBlockRead(struct FileSystem *fs)
{
    int ret = 1;
    int64_t count = 0;
    if (fs == NULL) {
        ret = -1;
        goto fail;
    }

    if (lseek(fs->fd, 1024, SEEK_SET) < 0) {
	printf("Seek failed\n");
	ret = -1;
	goto fail;
    }

    count = read(fs->fd, &(fs->super), sizeof(struct ext4_super_block));
    if (count != sizeof(struct ext4_super_block)) {
        printf("read fail: actual=%ld, size=%d\n", count, sizeof(struct ext4_super_block));
        ret = -1;
        goto fail;
    }

fail:
    return ret;
}

int FileSystemInit(struct FileSystem *fs, char *path) 
{
    int ret = 1;
    int fd = -1;

    if (fs == NULL) {
        printf("NULL pointer\n");
        ret = -1;
        goto fail;
    }
    memset(fs, 0, sizeof(struct FileSystem));

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("Open file failed\n");
        ret = -1;
        goto fail;
    }
    fs->fd = fd;

    fs->block_size = (EXT4_MIN_BLOCK_SIZE << fs->super.s_log_block_size);
    fs->descriptor_per_block = fs->block_size / fs->super.s_desc_size; // Assume it has 64bit feature
    fs->itable_block_per_group = div_ceil(fs->super.s_inodes_per_group * fs->super.s_inode_size,  fs->block_size); // Did not checkt s_rev_level
    fs->cluster_block_ratio = 1 << (fs->super.s_log_cluster_size - fs->super.s_log_block_size);

    return ret;
fail:
    free(fs);
    return ret;
}

int FileSystemRelease(struct FileSystem *fs)
{
    int ret = 1;
    if (fs == NULL) {
        return -1;
    }

    ret = close(fs->fd);
    if (ret < 0) {
        printf("Close failed\n");
        goto fail;
    }

    free(fs);
    return ret;
fail:
    printf("Release failed\n");
    free(fs);
    return ret;
}
