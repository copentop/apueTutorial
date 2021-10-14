#include "../apue.h"
#include <fcntl.h>

/**
 * 
 * 运行：./a.out & 
 */
int
main(void)
{
    if (open("bar.log", O_RDWR) < 0)
        err_sys("open error");

    if (unlink("bar.log") < 0)
        err_sys("unlink error");

    printf("---- file unlinked\n");

    sleep(15);
    printf("---- done\n");
    exit(0);
}