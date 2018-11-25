#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/kdev_t.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>
#include <asm/io.h>

#define DRIVER_AUTHOR	"CAUCSE KCH"
#define DRIVER_DESC	"driver for LCD on FPGA"

#define LCD_NAME	"lcd"
#define LCD_MODULE_VERSION	"LCD V1.0"
#define LCD_ADDR	0x08000090
#define LCD_REG_SIZE	0x01
#define NUM_CHARS	32
#define LCD_MAGIC	0xBC
#define LCD_SET_CURSOR_POS	_IOW(LCD_MAGIC, 0, int)
#define LCD_CLEAR	_IO(LCD_MAGIC, 1)

static unsigned char* lcd_ioremap = NULL;
static size_t lcd_cursor_pos = 0;

int lcd_open(struct inode* inodep, struct file* filep)
{
	struct resource *reg;
	reg = request_mem_region((unsigned long) LCD_ADDR, LCD_REG_SIZE * NUM_CHARS, LCD_NAME);
	if (reg == NULL)
	{
		printk(KERN_ERR "Failed to get 0x%x\n", (unsigned int) LCD_ADDR);
		return -EBUSY;
	}
	lcd_ioremap = ioremap(LCD_ADDR, LCD_REG_SIZE * NUM_CHARS);

	return 0;
}

int lcd_release(struct inode* inodep, struct file* filep)
{
	if(!lcd_ioremap)
	{
		return 0;
	}

	iounmap(lcd_ioremap);
	release_mem_region((unsigned long) LCD_ADDR, LCD_REG_SIZE * NUM_CHARS);

	return 0;
}

void __lcd_write_chars(unsigned short cc, int pos)
{
	if(pos>=0 && pos < NUM_CHARS)
	{
		iowrite16(cc, lcd_ioremap + pos);
	}
}

void lcd_clear()
{
	int i;
	unsigned short out;
	char buf[NUM_CHARS];

	memset(buf, ' ', NUM_CHARS);

 for(i=0; i< 32; i+=2)
                {
                        out = (((unsigned short) buf[i]) << 8 ) & 0xFF00;
                        out = out | (buf[i+1] & 0x00FF);

                        __lcd_write_chars(out, lcd_cursor_pos);

                        lcd_cursor_pos += 2;
                        lcd_cursor_pos = lcd_cursor_pos % NUM_CHARS;
                }


	__lcd_write_chars(out, 0);	
}

ssize_t lcd_write_chars(struct file* filep, const char* data, size_t length, loff_t* off_what)
{
	int i, ret;
	char buf[NUM_CHARS];
	unsigned short out;

	length = (length > NUM_CHARS) ? NUM_CHARS : length;

	memset(buf, ' ', NUM_CHARS);
	ret  = copy_from_user(buf, data, length);

	if(ret < 0) 
	{
		printk(KERN_ERR "data copy from userspace failed\n");
		return -EFAULT;
	}

	if(length > 0)
	{
		for(i=0; i< length; i+=2)
		{
			out = (((unsigned short) buf[i]) << 8 ) & 0xFF00;
			out = out | (buf[i+1] & 0x00FF);

			__lcd_write_chars(out, lcd_cursor_pos);

			lcd_cursor_pos += 2;
			lcd_cursor_pos = lcd_cursor_pos % NUM_CHARS;
		}
	}

	return length;
}



static long lcd_ioctl(struct file* file, unsigned int cmd, unsigned long data)
{
	int ret, param;

	switch (cmd)
	{
		case LCD_SET_CURSOR_POS:
			ret = copy_from_user(&param, (void*) data, _IOC_SIZE(LCD_SET_CURSOR_POS));
			if(ret < 0)
			{
				printk(KERN_ERR "data copy from userspace failed \n");
				return -EFAULT;
			}

			if(param < 0 || param >= NUM_CHARS)
			{
				printk(KERN_ERR "invalid cursor position \n");
				return -EFAULT;
			}

			lcd_cursor_pos = (param >> 1) << 1;
			printk(KERN_DEBUG "param: %d, pos: %d\n", param, lcd_cursor_pos);
			break;
		case LCD_CLEAR:
			lcd_clear();	
			break;
	}

	return 0;
}

static struct file_operations lcd_fops = 
{
	.owner	= THIS_MODULE,
	.open	= lcd_open,
	.write	= lcd_write_chars,
	.unlocked_ioctl	= lcd_ioctl,
	.release	= lcd_release,
};

static struct miscdevice lcd_driver = 
{
	.fops	= &lcd_fops,
	.name	= LCD_NAME,
	.minor	= MISC_DYNAMIC_MINOR,
};

int lcd_init(void)
{
	misc_register(&lcd_driver);
	printk(KERN_INFO "driver : %s driver init\n", LCD_NAME);

	return 0;
}

int lcd_exit(void)
{
	misc_deregister(&lcd_driver);
	printk(KERN_INFO "driver: %s driver exit\n", LCD_NAME);
}

module_init(lcd_init);
module_exit(lcd_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
