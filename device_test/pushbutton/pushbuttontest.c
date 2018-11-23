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
    unsigned short res;

    dev = open("/dev/pushbutton", O_RDWR);

    if (dev != -1)
    {
        read(dev, &res, sizeof(unsigned short));
        fprintf(stderr, "VALUE : %d\n", res);
        close(dev);
    }
    else {
        fprintf(stderr, "error opening device\n");
        return -1;
    }
    return 0;
}
