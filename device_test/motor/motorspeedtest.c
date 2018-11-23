#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(int argc, char **argv)
{
    int dev;
    unsigned char buff;
    if (argc <= 1)
    {
        fprintf(stderr, "usage: %s num\n", argv[0]);
        return -1;
    }

    dev = open("/dev/motorspeed", O_WRONLY);

    if (dev != -1)
    {
        buff = (int) strtol(argv[1], NULL, 10);
        write(dev, &buff, sizeof(buff));
        close(dev);
    } else {
        fprintf(stderr, "error opening device\n");
        return -1;
    }
    return 0;
}
