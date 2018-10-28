
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* firstdrvtest on
  * firstdrvtest off
  */

int main(int argc, char **argv)
{
    int fd;
    int val = 1;
    int  i = 0;
    fd = open("/dev/xyz", O_RDWR);

    if (fd < 0) {
        printf ("can't open!");
        return 0;
    }

    if (argc != 2) {
        printf ("%s <on|off>", argv[0]);
        return 0;
    }

    if (strcmp(argv[1] , "on") == 0) {
        val = 1;
    } else {
        val = 0;
    }

    /* 打印传递进来的参数  */
    for (i = 0; i < argc; i++) {
        printf ("argc[%d] = %s \n", i, argv[i]);
    }
    
    printf("%d\n", fd);
    
	write(fd, &val, 4);
	return 0;
}

