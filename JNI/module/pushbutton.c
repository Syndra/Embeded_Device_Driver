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
#define PUSHB_REG_SIZE 0x12
#define NUM_PUSHBS 9

static unsigned short *pushb_ioremap = NULL;

int pushb_open(struct inode *inodep, struct file *filep)
{
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

ssize_t pushb_read(struct file *filep, void* data, size_t length, loff_t *off_what)
{
	int i, j, ret;
  unsigned short out = 100;
	unsigned int res = 0;

	for(i = 0; i < NUM_PUSHBS; pushb_ioremap++)
	{
		out = ioread16(pushb_ioremap);
		printk(KERN_INFO "BUTTON STATUS(NUM %d) = %d\n", i, out);
		i++;

		if(out == 1)
			res++;
		res*=10;
	}
	res/=10;
	//reset pointers
	for(i = 0; i < NUM_PUSHBS; i++)
	{
		pushb_ioremap--;
	}

	printk(KERN_INFO "BUTTON TOTAL = %d\n", res);

	ret = copy_to_user(data, &res, sizeof(unsigned int));

	if (ret < 0)
	{
		printk(KERN_ERR "data copy form userspace failed \n");
		return 0;
	}

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
