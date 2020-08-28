#include <stdio.h>
#include <stdlib.h>
#include <endian.h>

#include "filesystem.h"

int main(int argc, char **argv)
{
    int ret = 0;
    int feature = 0;
    int num = 0;
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
    // FileSystemPrint(fs);

    sscanf(argv[2], "%d", &feature);
    switch(feature) {
        case 1:
            GroupDescriptorsPrint(fs);
            break;
        case 2:
            sscanf(argv[3], "%d", &num);
            GroupDescriptorsPrintBynum(fs, num);
            break;
        case 3:
            sscanf(argv[3], "%d", &num);
            InodePrintBynum(fs, num);
            break;
        case 4:
            sscanf(argv[3], "%d", &num);
            InodeStatusPrintBynum(fs, num);
            break;
        case 5:
            sscanf(argv[3], "%d", &num);
            BlockStatusPrintBynum(fs, num);
            break;
        case 6:
            sscanf(argv[3], "%d", &num);
            XattrPrintBynum(fs, num);
            break;
        default:
            printf("Unknown feature\n");
            break;
    }

end:
    FileSystemRelease(fs);
    return ret;
}
