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
#define DRIVER_DESC	"driver for dot matrix on FPGA"

#define DMAT_NAME	"dotmatrix"
#define DMAT_MODULE_VERSION	"dotmatrix V1.0"
#define	DMAT_ADDR	0x08000210
#define DMAT_REG_SIZE	0x02
#define NUM_SCANLINES	10

static unsigned char dmat_fontmap_clear[NUM_SCANLINES] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00	
};
static unsigned char dmat_fontmap_decimal[10][NUM_SCANLINES] = 
{
	{0x3E,0x7F,0x63,0x73,0x73,0x6F,0x67,0x63,0x7F,0x30}, // 0
	{0x0C,0x1C,0x1C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E}, // 1
	{0x66,0x66,0x66,0x66,0x66,0x66,0x7F,0x7F,0x06,0x06}, // 2
	{0x7F,0x7F,0x60,0x60,0x7E,0x7F,0x03,0x03,0x7F,0x7E}, // 3
	{0x7E,0x7F,0x03,0x03,0x3F,0x7E,0x60,0x60,0x7F,0x7F}, // 4
	{0xFE,0x7F,0x03,0x03,0x7F,0x7F,0x03,0x03,0x7F,0x70}, // 5
	{0x60,0x60,0x60,0x60,0x7E,0x7F,0x63,0x63,0x7F,0x3E}, // 6 
	{0x7F,0x7F,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
	{0x3E,0x7F,0x63,0x63,0x7F,0x7F,0x63,0x63,0x7F,0x3E}, // 8 
	{0x3E,0x7F,0x63,0x63,0x7F,0x3F,0x03,0x03,0x03,0x03} // 9 
};

static unsigned short *dmat_ioremap = NULL;

int dmat_open(struct inode *inodep, struct file *filep)
{
	int alloc_size;
	struct resource *reg;

	alloc_size = DMAT_REG_SIZE * NUM_SCANLINES;

	reg = request_mem_region((unsigned long) DMAT_ADDR, alloc_size, DMAT_NAME);
	if(reg == NULL)
	{
		printk(KERN_ERR "Failed to get 0x%x\n", (unsigned int) DMAT_ADDR);
		return -EBUSY;
	}
	dmat_ioremap = ioremap(DMAT_ADDR, alloc_size);

	return 0;
}

int dmat_release(struct inode *inodep, struct file* filep)
{
	int alloc_size;

	if(!dmat_ioremap)
		return 0;

	alloc_size = DMAT_REG_SIZE * NUM_SCANLINES;

	iounmap(dmat_ioremap);
	release_mem_region((unsigned long) DMAT_ADDR, alloc_size);

	return 0;
}

static void __dmat_write_from_int(unsigned char *data)
{
	int i;
	unsigned short out;

	for (i=0; i<NUM_SCANLINES; i++)
	{
		out = 0x007F & data[i];
		iowrite16(out, (dmat_ioremap + i));
	}
}

ssize_t dmat_write_from_int(struct file *filep, const char *data, size_t length, loff_t *off_what)
{
	int num, ret;

	ret = copy_from_user(&num, data, sizeof(int));
	if(ret<0)
	{
		printk(KERN_ERR "data copy form userspace failed \n");
		return -EFAULT;
	}

	if(num >= 0 && num <= 9)
	{
		__dmat_write_from_int(dmat_fontmap_decimal[num]);
	}
	else
	{
		__dmat_write_from_int(dmat_fontmap_clear);
	}

	return length;
}

static struct file_operations dmat_fops = 
{
	.owner	= THIS_MODULE,
	.open	= dmat_open,
	.write	= dmat_write_from_int,
	.release= dmat_release,
};

static struct miscdevice dmat_driver = 
{
	.fops	= &dmat_fops,
	.name	= DMAT_NAME,
	.minor	= MISC_DYNAMIC_MINOR,
};

int dmat_init(void)
{
	misc_register(&dmat_driver);
	printk(KERN_INFO "driver: %s driver init\n", DMAT_NAME);

	return 0;
}

void dmat_exit(void)
{
	misc_deregister(&dmat_driver);
	printk(KERN_INFO "driver: %s driver exit\n", DMAT_NAME);
}

module_init(dmat_init);
module_exit(dmat_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
