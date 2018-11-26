#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/kdev_t.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <asm/ioctl.h>
#define DRIVER_AUTHOR	"CAUCSE Lenny"
#define DRIVER_DESC	"driver for 8-LEDs FPGA"
#define LED_NAME	"led"
#define LED_MODULE_VERSION	"LED v1.0"
#define LED_ADDR	0x08000016
#define LED_REG_SIZE	0x01
 
static unsigned long *led_ioremap = NULL;
int led_open(struct inode *inodep, struct file *filep)
{
    struct resource *reg;

    reg = request_mem_region((unsigned long) LED_ADDR, LED_REG_SIZE, LED_NAME);
    if (reg == NULL) {
        printk(KERN_WARNING "Cannot get 0x%x\n", (unsigned int) LED_ADDR);
        return -EBUSY;
    }
    led_ioremap = ioremap(LED_ADDR, LED_REG_SIZE);
    printk("Led Module is Open.\n");
    return 0;
}
int led_release(struct inode *inodep, struct file *filep)
{
    if (!led_ioremap) {
        return 0;
    }
    iounmap(led_ioremap);
    release_mem_region((unsigned long) LED_ADDR, LED_REG_SIZE);
    printk("Led Module is release.\n");
    return 0;
}
ssize_t led_write_byte(struct file *inodep, const char *data, size_t length,
    loff_t *off_what)
{
    unsigned char c;

    get_user(c, data);

    iowrite8(c, led_ioremap);
    return length;
}
static struct file_operations led_fops = {
    .owner	= THIS_MODULE,
    .open	= led_open,
    .write	= led_write_byte,
    .release	= led_release,
};
static struct miscdevice led_driver = {
    .fops = &led_fops,
    .name = LED_NAME,
    .minor = MISC_DYNAMIC_MINOR,
};
int led_init(void)
{
    misc_register(&led_driver);
    printk(KERN_INFO "driver: %s driver init\n", LED_NAME);
    return 0;
}
void led_exit(void)
{
    misc_deregister(&led_driver);
    printk(KERN_INFO "driver: %s driver exit\n", LED_NAME);
}
module_init(led_init);
module_exit(led_exit);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
