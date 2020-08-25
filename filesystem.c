#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "filesystem.h"

uint64_t TotalBlockCountGet(struct FileSystem *fs)
{
    if (fs == NULL) {
        return -1;
    }
    return fs->super.s_blocks_count_lo | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)fs->super.s_blocks_count_hi) << 32 : 0);
}

uint32_t div_ceil(uint32_t dividen, uint32_t divisor)
{
    if (dividen == 0) {
        return 0;
    }

    return (dividen - 1) / divisor + 1;
}

bool HasRoot(uint32_t num, uint32_t root)
{
    while (num >= root) {
        if (num == root) {
            return true;
        }
        num = num / root;
    }
    return false;
}

bool GroupHasSuperblock(uint32_t group, struct FileSystem *fs)
{
    if (group == 0) {
        return true;
    }
    if (HAS_COMPAT_FEATURE(fs->super, EXT4_FEATURE_COMPAT_SPARSE_SUPER2)) {
        if (group == fs->super.s_backup_bgs[0] || group == fs->super.s_backup_bgs[1]) {
            return true;
        }
        return false;
    }
    if ((group <= 1) || !HAS_RO_COMPAT_FEATURE(fs->super, EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER)) {
        return true;
    }
    if (!(group & 1)) {
        return false;
    }
    if (HasRoot(group, 3) || (HasRoot(group, 5)) || HasRoot(group, 7)) {
        return true;
    }
    return false;
}

/*
 * Traverse all groups, print it
 */
void GroupsPrint(struct FileSystem *fs)
{
    uint32_t i = 0;
    uint64_t start = 0;
    for (i = 0; i < fs->group_count; i++) {
        start = GroupLocationGet(fs, i);
        printf("Group %lu:\n", i);
        printf("\t start at: %llu\n", start);
    }
}

/*
 * Given group number, get the location
 */
uint64_t GroupLocationGet(struct FileSystem *fs, uint32_t group_num)
{
    uint32_t group_block = fs->super.s_first_data_block + group_num * fs->super.s_blocks_per_group;

    /* Special case, if the block size is 1KB */
    if (group_num == 0 && fs->block_size == 1024 && fs->cluster_block_ratio > 1) {
        group_block += 1;
    }

    return group_block;
}

/*
 * Print the info of the filesytem
 */
void FileSystemPrint(struct FileSystem *fs)
{
    if (fs == NULL) {
        printf("FileSystemPrint cannot take a null pointer\n");
        return;
    }
    printf("Super block:\n");
    printf("\tinode count = %llu\n", fs->inode_count);
    printf("\tblock count = %llu\n", fs->block_count);
    printf("\tblock size = %llu\n", fs->block_size);
    printf("\tgroup size = %llu\n", fs->group_size);
    printf("\tgroup descriptor size = %llu\n", fs->descriptor_size);
    printf("\tcluster block ratio = %llu\n", fs->cluster_block_ratio);
    printf("\tgroup count = %llu\n", fs->group_count);
    printf("\tdescriptor per block = %llu\n", fs->descriptor_per_block);
    printf("\titable block per group = %llu\n", fs->itable_block_per_group);
    printf("\tdescriptor used block count = %llu\n", fs->descriptor_used_block_count);
}

/*
 * Read the super block, skip the front 1024 bytes
 *
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

    ret = SuperBlockRead(fs);
    if (ret < 0) {
        printf("Read super block failed\n");
        goto fail;
    }

    fs->block_size = (EXT4_MIN_BLOCK_SIZE << fs->super.s_log_block_size);
    fs->inode_count = fs->super.s_inodes_count;
    fs->block_count = TotalBlockCountGet(fs);
    fs->blocks_per_group = fs->super.s_blocks_per_group;
    fs->group_size = fs->blocks_per_group * fs->block_size;
    fs->descriptor_size = (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT)) ? fs->super.s_desc_size : EXT4_MIN_DESC_SIZE;
    fs->cluster_block_ratio = 1 << (fs->super.s_log_cluster_size - fs->super.s_log_block_size);
    fs->group_count = div_ceil(TotalBlockCountGet(fs) - fs->super.s_first_data_block, fs->super.s_blocks_per_group);
    fs->descriptor_per_block = fs->block_size / fs->descriptor_size;
    fs->descriptor_used_block_count = div_ceil(fs->group_count, fs->descriptor_per_block);
    fs->itable_block_per_group = div_ceil(fs->super.s_inodes_per_group * fs->super.s_inode_size,  fs->block_size); // Did not checkt s_rev_level

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
