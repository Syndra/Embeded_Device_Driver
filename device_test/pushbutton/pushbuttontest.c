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
    unsigned int res;
    int ret;

    dev = open("/dev/pushbutton", O_RDONLY);

    if (dev != -1)
    {
        ret = read(dev, &res, sizeof(unsigned int));
        fprintf(stderr, "VALUE : %d RETURN : %d\n", res, ret);
        close(dev);
    }
    else {
        fprintf(stderr, "error opening device\n");
        return -1;
    }
    return 0;
}
