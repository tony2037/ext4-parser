#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "filesystem.h"

int FileSystemInit(struct FileSystem *fs, char *path) 
{
    int ret = 1;
    int fd = -1;

    if (fs == NULL) {
        fs = malloc(sizeof(struct FileSystem));
    }
    memset(fs, 0, sizeof(struct FileSystem));

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("Open file failed\n");
        ret = -1;
        goto fail;
    }
    fs->fd = fd;

    return ret;
fail:
    free(fs);
    return ret;
};

int FileSystemRelease(struct FileSystem *fs)
{
    int ret = 1;
    ret = close(fs->fd);
    if (ret < 0) {
        printf("Close failed\n");
        goto fail;
    }

    return ret;
fail:
    printf("Release failed\n");
    return ret;
}
