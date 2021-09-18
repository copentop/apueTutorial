#include "../apue.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int
main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage a.out tmp.log\n");
        exit(0);
    }
    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("open %s fail \n", argv[1]);
        err_exit(errno, "open file err");
    }

    off_t of;
    if ((of = lseek(fd, -10, SEEK_END)) == -1) {
        printf("cannot seek\n");
        err_exit(errno, "lseek err");
    } else {
        printf("seek OK pos: %ld\n", of);
    }

    exit(0);
}