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
#define DRIVER_DESC "driver for motor on FPGA"

#define MOTOR_NAME "motor"
#define MOTOR_MODULE VERSION "motor V1.0"
#define MOTOR_ADDR 0x0800000C
#define MOTOR_REG_SIZE 0x06

static unsigned short *motor_ioremap = NULL;

int motor_open(struct inode *inodep, struct file *filep)
{
	struct resource *reg;

	reg = request_mem_region((unsigned long) MOTOR_ADDR, MOTOR_REG_SIZE, MOTOR_NAME);
	if (reg == NULL)
	{
		printk(KERN_ERR "Fail to get 0x%x\n", (unsigned int) MOTOR_ADDR);
		return -EBUSY;
	}
	motor_ioremap = ioremap(MOTOR_ADDR, MOTOR_REG_SIZE);

	return 0;
}

int motor_release(struct inode *inodep, struct file *filep)
{
	if(!motor_ioremap)
	{
		return 0;
	}

	iounmap(motor_ioremap);
	release_mem_region((unsigned long) MOTOR_ADDR, MOTOR_REG_SIZE);

	return 0;
}

static void __motor_write_from_int(int num)
{
	if(num == 768)
	{
		iowrite8(0x00, motor_ioremap);
		motor_ioremap++; motor_ioremap++;
		iowrite8(0xFF, motor_ioremap);
		motor_ioremap--; motor_ioremap--;
	}
	else
	{
		iowrite8(0x01, motor_ioremap);
		motor_ioremap++; motor_ioremap++;
		iowrite8(0x05, motor_ioremap);
		motor_ioremap--; motor_ioremap--;
	}
}

ssize_t motor_write_from_int(struct file *filep, const char *data, size_t length, loff_t *off_what)
{
	int i, num, ret;

	ret  = copy_from_user(&num, data, sizeof(int));
	if (ret < 0)
	{
		printk(KERN_ERR "data copy form userspace failed \n");
		return 0;
	}

	__motor_write_from_int(num);

	return length;
}

static struct file_operations motor_fops =
{
	.owner = THIS_MODULE,
	.open = motor_open,
	.write = motor_write_from_int,
	.release = motor_release,
};

static struct miscdevice motor_driver =
{
	.fops = &motor_fops,
	.name = MOTOR_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

int motor_init(void)
{
	misc_register(&motor_driver);
	printk(KERN_INFO "driver : %s driver init\n", MOTOR_NAME);

	return 0;
}

void motor_exit(void)
{
	misc_deregister(&motor_driver);
	printk(KERN_INFO "driver : %s driver exit\n", MOTOR_NAME);
}

module_init(motor_init);
module_exit(motor_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
