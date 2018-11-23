#include <linux/module.h>
#include <linux/init.h> 
#include <linux/kernel.h> 
#include <linux/miscdevice.h> 
#include <linux/fs.h> 
#include <linux/ioport.h> 
#include <linux/kdev_t.h> 
#include <linux/miscdevice.h> 
 
#include <asm/uaccess.h> 
#include <asm/ioctl.h> 
#include <asm/io.h>

#define DRIVER_AUTHOR "CAUCSE LJH"
#define DRIVER_DESC "driver for 7-Segment on FPGA" 

#define SSEG_NAME "7segment" 
#define SSEG_MODULE VERSION "7segment V1.0" 
#define SSEG_ADDR 0x08000004 
#define SSEG_REG_SIZE 0x02 
#define NUM_SSEGS 4 

static unsigned short *sseg_ioremap = NULL;

int sseg_open(struct inode *inodep, struct file *filep)
{
	struct resource *reg;

	reg = request_mem_region((unsigned long) SSEG_ADDR, SSEG_REG_SIZE, SSEG_NAME);
	if (reg == NULL)
	{
		printk(KERN_ERR "Fail to get 0x%x\n", (unsigned int) SSEG_ADDR);
		return -EBUSY;
	}
	sseg_ioremap = ioremap(SSEG_ADDR, SSEG_REG_SIZE);

	return 0;
}

int sseg_release(struct inode *inodep, struct file *filep)
{
	if(!sseg_ioremap)
	{
		return 0;
	}

	iounmap(sseg_ioremap);
	release_mem_region((unsigned long) SSEG_ADDR, SSEG_REG_SIZE);

	return 0;
}

static void __sseg_write_from_int(unsigned char *data)
{
	int i;
	unsigned short out = 0, bb = 0;

	for (i = 0; i <NUM_SSEGS; i++)
	{
		bb = data[i];
		out = out | (bb << (i * 4));
	}

	iowrite16(out, sseg_ioremap);
}

ssize_t sseg_write_from_int(struct file *filep, const char *data, size_t length, loff_t *off_what)
{
	int i, num, ret;
	unsigned char sseg_data[NUM_SSEGS] = {0, };

	ret  = copy_from_user(&num, data, sizeof(int));
	if (ret < 0)
	{
		printk(KERN_ERR "data copy form userspace failed \n");
		return 0;
	}

	if (num > 0)
	{
		printk(KERN_INFO "request num : %d\n", num);
		for (i = 0; i < NUM_SSEGS; i++)
		{
			sseg_data[i] = num % 10;
			num = num / 10;
		}
	}

	__sseg_write_from_int(sseg_data);

	return length;
}

static struct file_operations sseg_fops = 
{
	.owner = THIS_MODULE,
	.open = sseg_open,
	.write = sseg_write_from_int,
	.release = sseg_release,
};

static struct miscdevice sseg_driver = 
{
	.fops = &sseg_fops,
	.name = SSEG_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

int sseg_init(void)
{
	misc_register(&sseg_driver);
	printk(KERN_INFO "driver : %s driver init\n", SSEG_NAME);

	return 0;
}

void sseg_exit(void)
{
	misc_deregister(&sseg_driver);
	printk(KERN_INFO "driver : %s driver exit\n", SSEG_NAME);
}

module_init(sseg_init);
module_exit(sseg_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
