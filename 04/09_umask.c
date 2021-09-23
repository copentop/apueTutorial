#include "../apue.h"
#include <fcntl.h>
#include <stdio.h>
#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
int
main(void)
{
    mode_t m = umask(0);
    printf("%d \n",m);
    if (creat("foo.log", RWRWRW) < 0)
        err_sys("creat error for foo");

    m = umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    printf("%d \n",m);
    if (creat("bar.log", RWRWRW) < 0)
        err_sys("creat error for bar");
    exit(0);
}