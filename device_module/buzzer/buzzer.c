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

#define DRIVER_AUTHOR "CAUCSE TEAM FRIDAY"
#define DRIVER_DESC "driver for 7-Segment on FPGA"

#define BUZZER_NAME "buzzer"
#define BUZZER_MODULE VERSION "buzzer V1.0"
#define BUZZER_ADDR 0x08000070
#define BUZZER_REG_SIZE 0x01

static unsigned short *buzzer_ioremap = NULL;

int buzzer_open(struct inode *inodep, struct file *filep)
{
	struct resource *reg;

	reg = request_mem_region((unsigned long) BUZZER_ADDR, BUZZER_REG_SIZE, BUZZER_NAME);
	if (reg == NULL)
	{
		printk(KERN_ERR "Fail to get 0x%x\n", (unsigned int) BUZZER_ADDR);
		return -EBUSY;
	}
	buzzer_ioremap = ioremap(BUZZER_ADDR, BUZZER_REG_SIZE);

	return 0;
}

int buzzer_release(struct inode *inodep, struct file *filep)
{
	if(!buzzer_ioremap)
	{
		return 0;
	}

	iounmap(buzzer_ioremap);
	release_mem_region((unsigned long) BUZZER_ADDR, BUZZER_REG_SIZE);

	return 0;
}

static void __buzzer_write_from_int(int num)
{
  if(num == 0)
    iowrite8(0x00, buzzer_ioremap);
  else
    iowrite8(0x01, buzzer_ioremap);
}

ssize_t buzzer_write_from_int(struct file *filep, const char *data, size_t length, loff_t *off_what)
{
	int i, num, ret;

	ret  = copy_from_user(&num, data, sizeof(int));
	if (ret < 0)
	{
		printk(KERN_ERR "data copy form userspace failed \n");
		return 0;
	}

	__buzzer_write_from_int(num);

	return length;
}

static struct file_operations buzzer_fops =
{
	.owner = THIS_MODULE,
	.open = buzzer_open,
	.write = buzzer_write_from_int,
	.release = buzzer_release,
};

static struct miscdevice buzzer_driver =
{
	.fops = &buzzer_fops,
	.name = BUZZER_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

int buzzer_init(void)
{
	misc_register(&buzzer_driver);
	printk(KERN_INFO "driver : %s driver init\n", BUZZER_NAME);

	return 0;
}

void buzzer_exit(void)
{
	misc_deregister(&buzzer_driver);
	printk(KERN_INFO "driver : %s driver exit\n", BUZZER_NAME);
}

module_init(buzzer_init);
module_exit(buzzer_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
