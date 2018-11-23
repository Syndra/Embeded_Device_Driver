#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int dev, num;

	if (argc <= 1)
	{
		fprintf(stderr, "Usage : %s 1234\n", argv[0]);
		return -1;
	}

	dev = open("/dev/7segment", O_WRONLY);
	if (dev != -1)
	{
		num = (int) strtol(argv[1], NULL, 10);
		write(dev, &num, sizeof(num));
		close(dev);
	}
	else
	{
		fprintf(stderr, "error opening device\n");
		return -1;
	}

	return 0;
}
