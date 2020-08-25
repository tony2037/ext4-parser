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
        return 0;
    }
    return fs->super.s_blocks_count_lo | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)fs->super.s_blocks_count_hi) << 32 : 0);
}

uint64_t BlockBitmapLocationGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return pdesc->bg_block_bitmap_lo | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)pdesc->bg_block_bitmap_hi) << 32 : 0);
}

uint64_t InodeBitmapLocationGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return pdesc->bg_inode_bitmap_lo | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)pdesc->bg_inode_bitmap_hi) << 32 : 0);
}

uint64_t InodeTableLocationGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return pdesc->bg_inode_table_lo | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)pdesc->bg_inode_table_hi) << 32 : 0);
}

uint32_t FreeBlocksCountGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return pdesc->bg_free_blocks_count_lo | (uint32_t)pdesc->bg_free_blocks_count_hi << 16;
}

uint32_t UsedDirsCountGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return pdesc->bg_used_dirs_count_lo | (uint32_t)pdesc->bg_used_dirs_count_hi << 16;
}

uint32_t UnusedInodesCountGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return ((uint32_t)pdesc->bg_itable_unused_lo | (uint32_t)pdesc->bg_itable_unused_hi << 16);
}

uint32_t FreeInodesCountGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return pdesc->bg_free_inodes_count_lo | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)pdesc->bg_free_inodes_count_hi) << 32 : 0);
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
 * Given descriptor block number, get the location
 */
uint64_t GroupDescriptorLocationGet(struct FileSystem *fs, uint32_t descblock)
{
    uint32_t group_num = fs->descriptor_per_block * descblock;
    uint64_t block = 0;

    if (!HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_META_BG) || (descblock < fs->super.s_first_meta_bg)) {
        return GroupLocationGet(fs, 0) + 1 + descblock;
    }

    block = GroupLocationGet(fs, group_num);
    if (GroupHasSuperblock(group_num, fs)) {
        block += 1;
    }

    return block;
}

/*
 * Fetch all the Group Descriptors.
 * Firstly figure out how many blocks these Descriptors takes
 * Find the start location, and read data block by block
 */
uint64_t GroupDescriptorsFetch(struct FileSystem *fs)
{
    int ret = 0;
    char *buf = NULL;
    uint64_t block = 0;
    uint64_t first_meta_bg = 0;
    uint64_t i = 0, count = 0;

    fs->group_descriptors = (struct ext4_group_desc *) malloc(fs->descriptor_used_block_count * fs->block_size);
    buf = (char *)fs->group_descriptors;

    if (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_META_BG)) {
        first_meta_bg = fs->super.s_first_meta_bg;
        if (first_meta_bg > fs->descriptor_used_block_count) {
            first_meta_bg = fs->descriptor_used_block_count;
        }
    } else {
        first_meta_bg = fs->descriptor_used_block_count;
    }

    if (first_meta_bg > 0) {
        block = GroupDescriptorLocationGet(fs, 0);
        if (!BlockRead(fs, block, first_meta_bg, buf)) {
            ret = -1;
            goto fail;
        }

        buf += fs->block_size * first_meta_bg;
        count += fs->block_size * first_meta_bg;
    }

    for (i = first_meta_bg; i < fs->descriptor_used_block_count; i++) {
        block = GroupDescriptorLocationGet(fs, i);

        if (!BlockRead(fs, block, 1, buf)) {
            ret = -1;
            goto fail;
        }

        buf += fs->block_size;
        count += fs->block_size;
    }

    return count;
fail:
    free(fs->group_descriptors);
    return -1;
}

/*
 * Read blocks
 * @fs: FileSystem
 * @start: the block number begin to read
 * @num: How many blocks is gonna be read
 * @buf: buffer for reading
 */
uint64_t BlockRead(struct FileSystem *fs, uint64_t start, uint64_t num, char *buf)
{
    uint64_t ret = 1;
    int64_t count = 0;
    if (fs == NULL) {
        printf("BlockRead: fs is NULL\n");
        ret = 0;
        goto fail;
    }
    if (buf == NULL) {
        printf("BlockRead: buf is NULL");
        ret = 0;
        goto fail;
    }

    if (lseek(fs->fd, fs->block_size * start, SEEK_SET) < 0) {
        printf("BlockRead: Seek failed\n");
	ret = 0;
	goto fail;
    }

    count = read(fs->fd, buf, fs->block_size * num);
    if (count != fs->block_size * num) {
        printf("read fail: actual=%ld, size=%d\n", count, fs->block_size * num);
        ret = 0;
        goto fail;
    }

    return count;
fail:
    return ret;
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
 * Print the info of all group descriptors
 */
void GroupDescriptorsPrint(struct FileSystem *fs)
{
    uint64_t i = 0;
    if (fs == NULL) {
        printf("GroupDescriptorsPrint cannot take a null pointer\n");
        return;
    }

    for (i = 0; i < fs->group_count; i++) {
        GroupDescriptorsPrintBynum(fs, i);
    }
}

/*
 * givin the number of group, print the info of the group descriptor
 */
void GroupDescriptorsPrintBynum(struct FileSystem *fs, uint64_t num)
{
    struct ext4_group_desc *pdesc;
    if (fs == NULL) {
        printf("GroupDescriptorsPrint cannot take a null pointer\n");
        return;
    }
    if (num >= fs->group_count) {
        printf("Invalid Group number. This has to be 0 ~ %llu\n", fs->group_count);
        return;
    }

    pdesc = &(fs->group_descriptors[num]);
    printf("Group %llu:", num);
    printf(" block bitmap at %llu", BlockBitmapLocationGet(fs, pdesc));
    printf(", inode bitmap at %llu", InodeBitmapLocationGet(fs, pdesc));
    printf(", inode table at %llu\n", InodeTableLocationGet(fs, pdesc));
    printf("\t%lu free blocks", FreeBlocksCountGet(fs, pdesc));
    printf(", %lu free inodes", FreeInodesCountGet(fs, pdesc));
    printf(", %lu used directories", UsedDirsCountGet(fs, pdesc));
    printf(", %lu unused inodes\n", UnusedInodesCountGet(fs, pdesc));
    printf("\t[Checksum 0x%x]\n", pdesc->bg_checksum);
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

    if (GroupDescriptorsFetch(fs) < 0) {
        ret = -1;
        goto fail;
    }

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
