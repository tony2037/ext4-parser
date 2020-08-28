#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <endian.h>

#include "filesystem.h"

void Hexdump(char *buf, uint64_t len) {
    uint64_t row = len / 16;
    uint64_t remain = len - row * 16;
    uint64_t r = 0, i = 0;
    for (r = 0; r < row; r ++) {
        for (i = 0; i < 16; i++) {
            printf("%02hhx ", (unsigned char)buf[r * 16 + i]);
        }
        printf("\n");
    }

    if (remain > 0) {
        for (i = row * 16; i < len; i++) {
            printf("%02hhx ", (unsigned char)buf[i]);
        }
        printf("\n");
    }
}

uint64_t TotalBlockCountGet(struct FileSystem *fs)
{
    if (fs == NULL) {
        return 0;
    }
    return (uint64_t)le32toh(fs->super.s_blocks_count_lo) | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)le32toh(fs->super.s_blocks_count_hi)) << 32 : 0);
}

uint64_t BlockBitmapLocationGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return (uint64_t)le32toh(pdesc->bg_block_bitmap_lo) | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)le32toh(pdesc->bg_block_bitmap_hi)) << 32 : 0);
}

uint64_t InodeBitmapLocationGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return (uint64_t)le32toh(pdesc->bg_inode_bitmap_lo) | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)le32toh(pdesc->bg_inode_bitmap_hi)) << 32 : 0);
}

uint64_t InodeTableLocationGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return (uint64_t)le32toh(pdesc->bg_inode_table_lo) | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint64_t)le32toh(pdesc->bg_inode_table_hi)) << 32 : 0);
}

uint32_t FreeBlocksCountGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return (uint32_t)le16toh(pdesc->bg_free_blocks_count_lo) | (uint32_t)le16toh(pdesc->bg_free_blocks_count_hi) << 16;
}

uint32_t UsedDirsCountGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return (uint32_t)le16toh(pdesc->bg_used_dirs_count_lo) | (uint32_t)le16toh(pdesc->bg_used_dirs_count_hi) << 16;
}

uint32_t UnusedInodesCountGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return ((uint32_t)le16toh(pdesc->bg_itable_unused_lo) | (uint32_t)le16toh(pdesc->bg_itable_unused_hi) << 16);
}

uint32_t FreeInodesCountGet(struct FileSystem *fs, struct ext4_group_desc *pdesc)
{
    if (fs == NULL) {
        return 0;
    }
    if (pdesc == NULL) {
        return 0;
    }
    return (uint32_t)le16toh(pdesc->bg_free_inodes_count_lo) | (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_64BIT) ? 
            ((uint32_t)le16toh(pdesc->bg_free_inodes_count_hi)) << 16 : 0);
}

uint64_t div_ceil(uint64_t dividen, uint64_t divisor)
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

// TODO: merge GroupLocationGet and GroupDescriptorLocationGet
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
        // TODO: consider 1k
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

    // TODO: Loading consideration
    fs->group_descriptors = (struct ext4_group_desc *) malloc(fs->descriptor_used_block_count * fs->block_size);
    // TODO: initialize
    buf = (char *)fs->group_descriptors;

    if (HAS_INCOMPAT_FEATURE(fs->super, EXT4_FEATURE_INCOMPAT_META_BG)) {
        first_meta_bg = fs->super.s_first_meta_bg;
        if (first_meta_bg > fs->descriptor_used_block_count) {
            // TODO: this should not happen
            first_meta_bg = fs->descriptor_used_block_count;
        }
    } else {
        first_meta_bg = fs->descriptor_used_block_count;
    }

    // TODO: Fix, skip the previous block, see spec
    // TODO: merge two section below
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
 * Read bytes
 * @fs: FileSystem
 * @offset: the offset begin to read
 * @len: How many bytes is gonna be read
 * @buf: buffer for reading
 */
uint64_t BytesRead(struct FileSystem *fs, uint64_t offset, uint64_t len, char *buf)
{
    uint64_t ret = 1;
    uint64_t count = 0;
    if (fs == NULL) {
        printf("BytesRead: fs is NULL\n");
        ret = 0;
        goto fail;
    }
    if (buf == NULL) {
        printf("BytesRead: buf is NULL");
        ret = 0;
        goto fail;
    }

    if (lseek(fs->fd, offset, SEEK_SET) < 0) {
        printf("BytesRead: Seek failed\n");
	ret = 0;
	goto fail;
    }

    count = read(fs->fd, buf, len);
    if (count != len) {
        printf("read fail: actual=%llu, size=%llu\n", count, len);
        ret = len;
        goto fail;
    }

    return count;
fail:
    return ret;
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

    Hexdump((char *)pdesc, sizeof(struct ext4_group_desc));
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
 * givin an inode number, print the inode table
 */
void InodePrintTableBynum(struct FileSystem *fs, uint64_t num)
{
    if (num <= 0) {
        printf("Invalid inode number\n");
        return;
    }

    uint64_t group = (num - 1) / fs->inodes_per_group;
    uint64_t i = 0;

    for (i = 0; i < fs->inodes_per_group; i++) {
        InodePrintBynum(fs, i + group * fs->inodes_per_group + 1);
    }
}

/*
 * givin an inode number, print the inode
 */
void InodePrintBynum(struct FileSystem *fs, uint64_t num)
{
    if (num <= 0) {
        printf("Invalid inode number\n");
        return;
    }

    struct ext4_inode inode;
    uint64_t count = 0;
    count = InodeGetBynum(fs, num, &inode);
    if (count == 0) {
        printf("Try to get inode: read failed\n");
        return;
    }

    Hexdump((char *)&inode, sizeof(struct ext4_inode));
    printf("Inode: %llu\t", num);
    printf("Type: %x\t", inode.i_mode);
    printf("Mode: %x\t", inode.i_mode);
    printf("Flag: %x\n", inode.i_flags);
    printf("Generation: %lu\t", inode.i_generation);
    printf("Version: 0x%x:%x\n", inode.i_version_hi, inode.osd1);
    printf("User: %u\tGroup: %u\tSize: %llu\n", 
            inode.i_uid, inode.i_gid, ((uint64_t)inode.i_size_lo | (uint64_t)inode.i_size_high << 32));
    printf("Links: %u\tBlockcount: %lu\n", 
            inode.i_links_count, inode.i_blocks_lo);
    printf("ctime: 0x%x:%x\n", inode.i_ctime_extra, inode.i_ctime);
    printf("atime: 0x%x:%x\n", inode.i_atime_extra, inode.i_atime);
    printf("mtime: 0x%x:%x\n", inode.i_mtime_extra, inode.i_mtime);
    printf("crtime: 0x%x:%x\n", inode.i_crtime_extra, inode.i_crtime);
    printf("Size of extra inode fields: %u\n", inode.i_extra_isize);
}

/*
 * givin an inode number, get the inode
 */
uint64_t InodeGetBynum(struct FileSystem *fs, uint64_t num, struct ext4_inode *pinode)
{
    if (num <= 0) {
        printf("Invalid inode number\n");
        return 0;
    }

    // TODO: use macro to handler
    uint64_t group = (num - 1) / fs->inodes_per_group;
    struct ext4_group_desc *pdesc = &(fs->group_descriptors[group]);
    uint64_t location = InodeTableLocationGet(fs, pdesc);
    uint64_t offset = location * fs->block_size + (num - 1) * le16toh(fs->super.s_inode_size);
    uint64_t count = 0;

    count = BytesRead(fs, offset, sizeof(struct ext4_inode), (char *)pinode);
    if (count == 0) {
        printf("Try to get inode: read failed\n");
    }
    return count;
}

/*
 * givin an inode number, get the inode bitmap
 */
uint64_t InodeBitmapGetBynum(struct FileSystem *fs, uint64_t num, char *buf)
{
    if (buf == NULL) {
        goto fail;
    }
    if (num <= 0) {
        printf("Invalid inode number\n");
        goto fail;
    }

    uint64_t group = (num - 1) / fs->inodes_per_group;
    struct ext4_group_desc *pdesc = &(fs->group_descriptors[group]);
    uint64_t location = InodeBitmapLocationGet(fs, pdesc);
    uint64_t count = 0;

    count = BytesRead(fs, location * fs->block_size, fs->block_size, buf);
    if (count == 0) {
        printf("Try to get inode: read failed\n");
        goto fail;
    }
    return count;

fail:
    return 0;
}

int InodeStatusGetBynum(struct FileSystem *fs, uint64_t num)
{
    if (fs == NULL) {
        return -1;
    }
    if (num <= 0) {
        printf("Invalid inode number\n");
        return -1;
    }

    // TODO: macro
    uint64_t group = (num - 1) / fs->inodes_per_group;
    struct ext4_group_desc *pdesc = &(fs->group_descriptors[group]);
    int ret = 0;
    char buf[fs->block_size];
    uint64_t count = 0;
    uint64_t byte = 0, shift = 0;

    if (le16toh(pdesc->bg_flags) & EXT4_BG_INODE_UNINIT) {
        ret = 2;
        return ret;
    }

    count = InodeBitmapGetBynum(fs, num, buf);
    if (count == 0) {
        return -1;
    }

    byte = (num - 1) / 8;
    shift = num - 1 - byte * 8;
    ret = (buf[byte] >> shift) & 0x1;

    return ret;
}

void InodeStatusPrintBynum(struct FileSystem *fs, uint64_t num)
{
    if (num <= 0) {
        printf("Invalid inode number\n");
        return;
    }
    int ret = InodeStatusGetBynum(fs, num);
    switch(ret) {
        case 1:
            printf("Inode %llu marked in use\n", num);
            break;
        case 0:
            printf("Inode %llu not in use\n", num);
            break;
        case 2:
            printf("Inode table/bitmap uninitialized\n");
            break;
        case -1:
            printf("Get inode status failed\n");
            break;
        default:
            printf("Unknown status\n");
            break;
    }
}

/*
 * givin an block number, get the block bitmap
 */
uint64_t BlockBitmapGetBynum(struct FileSystem *fs, uint64_t num, char *buf)
{
    if (buf == NULL) {
        goto fail;
    }
    if (num <= 0) {
        printf("Invalid block number\n");
        goto fail;
    }

    uint64_t group = num / fs->blocks_per_group;
    struct ext4_group_desc *pdesc = &(fs->group_descriptors[group]);
    uint64_t location = BlockBitmapLocationGet(fs, pdesc);
    uint64_t count = 0;

    count = BytesRead(fs, location * fs->block_size, fs->block_size, buf);
    if (count == 0) {
        printf("Try to get block bitmap: read failed\n");
        goto fail;
    }
    return count;

fail:
    return 0;
}

int BlockStatusGetBynum(struct FileSystem *fs, uint64_t num)
{
    if (fs == NULL) {
        return -1;
    }
    if (num <= 0) {
        printf("Invalid Block number\n");
        return -1;
    }

    // TODO: macro state
    uint64_t group = (num - 1) / fs->inodes_per_group;
    struct ext4_group_desc *pdesc = &(fs->group_descriptors[group]);
    int ret = 0;
    char buf[fs->block_size];
    uint64_t count = 0;
    uint64_t byte = 0, shift = 0;

    if (le16toh(pdesc->bg_flags) & EXT4_BG_BLOCK_UNINIT) {
        ret = 2;
        return ret;
    }

    count = BlockBitmapGetBynum(fs, num, buf);
    if (count == 0) {
        return -1;
    }
    byte = (uint8_t)num / 8;
    shift = num - byte * 8;
    ret = (buf[byte] >> shift) & 0x1;

    return ret;
}

void BlockStatusPrintBynum(struct FileSystem *fs, uint64_t num)
{
    if (num <= 0) {
        printf("Invalid Block number\n");
        return;
    }
    int ret = BlockStatusGetBynum(fs, num);
    switch(ret) {
        case 1:
            printf("Block %llu marked in use\n", num);
            break;
        case 0:
            printf("Block %llu not in use\n", num);
            break;
        case 2:
            printf("Block bitmap uninitialized\n");
            break;
        case -1:
            printf("Get Block status failed\n");
            break;
        default:
            printf("Unknown status\n");
            break;
    }
}

/*
 * givin an inode number, print the Xattr
 */
void XattrPrintBynum(struct FileSystem *fs, uint64_t num)
{
    struct ext4_inode inode;
    struct ext4_xattr_ibody_header *ihdr;
    struct ext4_xattr_header *hdr;
    uint64_t count = 0;
    uint64_t aclblock = 0;
    char buf[fs->block_size];

    if (num <= 0) {
        printf("Invalid inode number\n");
        goto fail;
    }

    memset(&inode, 0, sizeof(struct ext4_inode));
    count = InodeGetBynum(fs, num, &inode);
    if (count == 0) {
        printf("Get Inode failed\n");
        goto fail;
    }

    if (inode.i_extra_isize) {
        // get ibody header
        // TODO: see magic number in superblock
        ihdr = IHDR(inode, &inode);
        if (le32toh(ihdr->h_magic) == EXT4_XATTR_MAGIC) {
            XattrentryAllPrint((struct ext4_xattr_entry *)((void *)ihdr + sizeof(struct ext4_xattr_ibody_header)));
        }
    }
    if ((uint64_t)le32toh(inode.i_file_acl_lo) | (uint64_t)le16toh(inode.osd2.linux2.l_i_file_acl_high) << 32) {
        // goto other block
        // TODO: Consider hdr->h_blocks != 1
        aclblock = (uint64_t)le32toh(inode.i_file_acl_lo) | (uint64_t)le16toh(inode.osd2.linux2.l_i_file_acl_high) << 32;
        count = BlockRead(fs, aclblock, 1, buf);
        hdr = (struct ext4_xattr_header *)buf;
        if (le32toh(hdr->h_magic) == EXT4_XATTR_MAGIC) {
            XattrentryAllPrint((struct ext4_xattr_entry *)((void *)hdr + sizeof(struct ext4_xattr_header)));
        }
    }
fail:
    return;
}

/*
 * Given an entry, traverse all the entries and print it.
 *
 */
void XattrentryAllPrint(struct ext4_xattr_entry *entry)
{
    char prefix[10] = {0};
    while (entry->e_name_len != 0 || entry->e_name_index != 0 ||
            entry->e_value_offs != 0 || entry->e_value_inum != 0) {
        memset(prefix, 0, 10);
        INDEX_TO_STRING(entry->e_name_index, prefix);
        // TODO: fix, value
        printf("%s%s = %x\n", prefix, entry->e_name, (uint32_t)entry->e_hash);
        entry = EXT4_XATTR_NEXT(entry);
    }
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

    // TODO: see spec, consider 1K
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
    fs->inodes_per_group = fs->super.s_inodes_per_group;
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
