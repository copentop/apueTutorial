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
    char * buf = malloc(10);
    if (buf == NULL) {
        printf("malloc error");
        exit(0);
    }

    ssize_t n = read(fd, buf, 5);
    if (n < 0) {
        err_exit(errno, "read file err");
    }
    printf("read val: %s \n", buf);


    exit(0);
}