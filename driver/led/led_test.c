
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

    printf("-------------\n");
    
    fd = open(argv[1], O_RDWR);

    printf("file discrib %d\n", fd);

    if (fd < 0) {
        printf ("can't open!");
        return 0;
    }

    if (argc != 3) {
        printf ("%s <on|off>", argv[0]);
        printf("xxxxxxxxxxxxxxxxxx\n");
        return 0;
    }

    printf("xxxxxxxxxxxxxxxxxx\n");

    if (strcmp(argv[2] , "on") == 0) {
        val = 1;
    } else {
        val = 0;
    }

    printf("xxxxxxxxxxxxxxxxxx\n");

    /* 打印传递进来的参数  */
    for (i = 0; i < argc; i++) {
        printf ("argc[%d] = %s \n", i, argv[i]);
    }
    
    printf("%d\n", fd);
    
	write(fd, &val, 4);
	return 0;
}

