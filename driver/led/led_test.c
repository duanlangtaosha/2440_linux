
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
	fd = open("/dev/led", O_RDWR);
    printf("%d\n", fd);
    
	write(fd, &val, 4);
	return 0;
}

