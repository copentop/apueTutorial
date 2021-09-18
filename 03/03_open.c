#include "../apue.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char **argv) {
    mode_t filePerms;

    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;
    
    // 指定相对路径或绝对路径
    char base[10] = "./";
    // 打开目录
    int bfd = open(base,  O_RDONLY | O_DIRECTORY);
    if (bfd < 0) {
        err_exit(errno, "open dir err");
    }

    // 相对路径打开
    int fd = openat(bfd, "tmp1.log", O_RDWR | O_CREAT, filePerms);
    if (fd < 0) {
        err_exit(errno, "open file err");
    }
    
    // 写入
    ssize_t n = write(fd, "hello", 5);

    printf("%ld \n", n);

    // 关闭
    close(fd);
    close(bfd);
    

    exit(0);
}