#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define LCD_MAGIC	0xBC
#define LCD_SET_CURSOR_POS	_IOW(LCD_MAGIC, 0, int)
#define LCD_CLEAR		_IO(LCD_MAGIC, 1)

int main(int argc, char **argv)
{
	int dev, pos;

	if(argc < 3)
	{
		fprintf(stderr, "usage: %s 1234 ABCDE\n", argv[0]);
		return -1;
	}

	dev = open("/dev/lcd", O_WRONLY);
	if(dev != -1)
	{
		ioctl(dev, LCD_CLEAR, &pos, _IOC_SIZE(LCD_CLEAR));
		pos = 0;
		ioctl(dev, LCD_SET_CURSOR_POS, &pos, _IOC_SIZE(LCD_SET_CURSOR_POS));
		write(dev, argv[1], strlen(argv[1]));

		pos = 16;
		ioctl(dev, LCD_SET_CURSOR_POS, &pos, _IOC_SIZE(LCD_SET_CURSOR_POS));
		write(dev, argv[2], strlen(argv[2]));

		close(dev);
	}
	else
	{
		fprintf(stderr, "error opening device\n");
		return -1;
	}

	return 0;
}
