#include <stdio.h>
#include <stdlib.h>
#include <endian.h>

#include "filesystem.h"

int main(int argc, char **argv)
{
    int ret = 0;
    char *filename = NULL;
    struct FileSystem *fs = NULL;

    if (argc < 2) {
        printf("Usage:\n");
        printf("lsfs file\n");
        ret = -1;
        goto end;
    }
    filename = argv[1];
    fs = malloc(sizeof(struct FileSystem));

    ret = FileSystemInit(fs, filename);
    if (ret < 0) {
        printf("Initialize FileSystem failed\n");
        goto end;
    }

    FileSystemPrint(fs);
    GroupsPrint(fs);
    GroupDescriptorsPrint(fs);
    InodePrintBynum(fs, 12);
    InodeStatusPrintBynum(fs, 12);

end:
    FileSystemRelease(fs);
    return ret;
}
