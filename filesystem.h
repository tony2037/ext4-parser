#include <stdbool.h>
#include "ext4.h"

struct FileSystem {
    int fd;
    uint64_t block_size;
    uint64_t inode_count;
    uint64_t block_count;
    uint64_t blocks_per_group;
    uint64_t group_size;
    uint64_t descriptor_size;
    uint64_t cluster_block_ratio;
    uint64_t group_count;
    uint32_t descriptor_per_block;
    uint32_t itable_block_per_group;
    uint64_t descriptor_used_block_count;
    uint32_t csum_seed;
    struct ext4_super_block super;
};

bool HasRoot(uint32_t, uint32_t);
bool GroupHasSuperblock(uint32_t, struct FileSystem *);

int SuperBlockRead(struct FileSystem *);
int SuperBlockParse(struct FileSystem *);
int FileSystemInit(struct FileSystem *, char *);
int FileSystemRelease(struct FileSystem *);
void FileSystemPrint();
uint64_t GroupLocationGet(struct FileSystem *, uint32_t);
uint64_t GroupDescriptorLocationGet(struct FileSystem *, uint32_t);
void GroupsPrint(struct FileSystem *);
