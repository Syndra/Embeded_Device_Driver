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
#define DRIVER_DESC "driver for pushbutton on FPGA"

#define PUSHB_NAME "pushbutton"
#define PUSHB_MODULE VERSION "pushbutton V1.0"
#define PUSHB_ADDR 0x08000050
#define PUSHB_REG_SIZE 0x02
#define NUM_PUSHBS 9

static unsigned short *pushb_ioremap = NULL;

int pushb_open(struct inode *inodep, struct file *filep)
{
  printk(KERN_ERR "pushb open!\n");

	struct resource *reg;

	reg = request_mem_region((unsigned long) PUSHB_ADDR, PUSHB_REG_SIZE, PUSHB_NAME);
	if (reg == NULL)
	{
		printk(KERN_ERR "Fail to get 0x%x\n", (unsigned int) PUSHB_ADDR);
		return -EBUSY;
	}
	pushb_ioremap = ioremap(PUSHB_ADDR, PUSHB_REG_SIZE);

	return 0;
}

int pushb_release(struct inode *inodep, struct file *filep)
{
	if(!pushb_ioremap)
	{
		return 0;
	}

	iounmap(pushb_ioremap);
	release_mem_region((unsigned long) PUSHB_ADDR, PUSHB_REG_SIZE);

	return 0;
}

ssize_t pushb_read(struct file *filep, char *data, size_t length, loff_t *off_what)
{
	int i, num, ret;
  unsigned int out;
	unsigned char pushb_data[NUM_PUSHBS] = {0, };


  printk(KERN_INFO "BUTTON STATUS = %d\n", out);

  out = ioread16(pushb_ioremap);

  printk(KERN_INFO "BUTTON STATUS = %d\n", out);

	ret  = copy_to_user(&out, data, sizeof(unsigned int));

	return length;
}

static struct file_operations pushb_fops =
{
	.owner = THIS_MODULE,
	.open = pushb_open,
	.read = pushb_read,
	.release = pushb_release,
};

static struct miscdevice pushb_driver =
{
	.fops = &pushb_fops,
	.name = PUSHB_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

int pushb_init(void)
{
	misc_register(&pushb_driver);
	printk(KERN_INFO "driver : %s driver init\n", PUSHB_NAME);

	return 0;
}

void pushb_exit(void)
{
	misc_deregister(&pushb_driver);
	printk(KERN_INFO "driver : %s driver exit\n", PUSHB_NAME);
}

module_init(pushb_init);
module_exit(pushb_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
