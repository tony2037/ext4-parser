#include <string.h>
#include <stdlib.h>

#include "filesystem.h"

int FileSystemInit(struct FileSystem *fs) 
{
    memset(fs, 0, sizeof(struct FileSystem));
    return 1;
};
