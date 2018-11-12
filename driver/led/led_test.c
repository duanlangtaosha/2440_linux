
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* firstdrvtest on
  * firstdrvtest off
  */


void print_usage(char *file)
{
    printf("Usage:\n");
    printf("%s <dev> <on|off>\n", file);
    printf("eg. \n");
    printf("%s /dev/leds on  \n", file);
    printf("%s /dev/led0 on  \n", file);
    printf("%s /dev/led0 off \n", file);
    printf("%s /dev/led1 on  \n", file);
    printf("%s /dev/led1 off \n", file);
}


int main(int argc, char **argv)
{
    int fd;
    int val = 1;
    int  i = 0;
    char *file_name = NULL;

    if (argc != 3) {
        print_usage(argv[0]);
        return 0;
    }
    
    file_name = argv[1];

    fd = open(file_name, O_RDWR);

    if (fd < 0) {
        printf ("can't open %s !", argv[1]);
        return 0;
    }

    if (strcmp(argv[2] , "on") == 0) {
        val = 0;
    } else {
        val = 1;
    }

    /* 打印传递进来的参数  */
    for (i = 0; i < argc; i++) {
        printf ("argc[%d] = %s \n", i, argv[i]);
    }
    
    printf("%d\n", fd);
    
	write(fd, &val, 4);
	return 0;
}

