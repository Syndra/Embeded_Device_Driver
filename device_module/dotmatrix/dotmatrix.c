#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/timer.h>

#include <asm/uaccess.h>
#include <asm/ioctl.h>
#include <asm/io.h>

#define DRIVER_AUTHOR	"CAUCSE KCH"
#define DRIVER_DESC	"driver for dot matrix on FPGA"
#define DRIVER_AUTHOR_ID	20133950

#define DMAT_NAME	"dotmatrix"
#define DMAT_MODULE_VERSION	"dotmatrix V1.0"
#define	DMAT_ADDR	0x08000210
#define DMAT_REG_SIZE	0x02
#define NUM_SCANLINES	10
#define NUM_KORNAME     83
#define BLANK_LINE_SHORT    82

#define NUM_CHESSPIECE 6
/*
DMAT DETAILS

0 = PAWN
1 = ROOK
2 = BISHOP
3 = KNIGHT
4 = QUEEN
5 = KING
*/

#define CHECK_LINE 48
#define CHECKMATE_LINE 74
#define STALEMATE_LINE 75
#define WIN_LINE 43
#define LOSE_LINE 44

#define CHECK_CONDITION_VAR 101
#define CHECKMATE_CONDITION_VAR 102
#define STALEMATE_CONDITION_VAR 103
#define WIN_CONDITION_VAR 104
#define LOSE_CONDITION_VAR 105

static int condition_lineNum[5] =
{
	48, 74, 75, 43, 44
};

static unsigned char dmat_fontmap_clear[NUM_SCANLINES] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static unsigned char dmat_fontmap_decimal[10][NUM_SCANLINES] =
{
	{0x3E,0x7F,0x63,0x73,0x73,0x6F,0x67,0x63,0x7F,0x3E}, // 0
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

static unsigned char dmat_fontmap_vertical_decimal_id[40] = {};

static unsigned char dmat_fontmap_vertical_decimal[10][NUM_SCANLINES/2] =
{
    {0x00,0x3E,0x41,0x41,0x3E}, // 0
    {0x00,0x44,0x42,0x7F,0x40}, // 1
    {0x00,0x79,0x49,0x49,0x4F}, // 2
    {0x00,0x49,0x49,0x49,0x7F}, // 3
    {0x00,0x0F,0x08,0x08,0x7F}, // 4
    {0x00,0x4F,0x49,0x49,0x79}, // 5
    {0x00,0x7F,0x49,0x49,0x79}, // 6
    {0x00,0x01,0x01,0x01,0x7F}, // 7
    {0x00,0x36,0x49,0x49,0x36}, // 8
    {0x00,0x4F,0x49,0x49,0x7F}, // 9
};

static unsigned char dmat_fontmap_kor_name[NUM_KORNAME] =
{
   0x7F,0x08,0x14,0x22,0x41,0x00,0x3E,0x41,0x41,0x41,0x3E,0x00,0x7F,0x02,0x0C,0x18,0x20,0x7F,0x00,0x3E,0x41,0x49,0x49,0x49,0x38,0x00,0x3E,0x41,0x41,0x41,0x22,0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x7E,0x09,0x09,0x09,0x7E,0x00,0x7F,0x02,0x0C,0x18,0x20,0x7F,0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x03,0x04,0x78,0x04,0x03,0x00,0x3F,0x40,0x40,0x40,0x3F,0x00,0x7F,0x02,0x0C,0x18,0x20,0x7F,0x00,0x3E,0x41,0x41,0x49,0x49,0x38,0x00
};

static unsigned char dmat_fontmap_vertical_CHECK[CHECK_LINE] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x3E,0x41,0x41,0x41,0x22,0x00,0x7F,0x08,0x08,0x08,
	0x7F,0x00,0x7F,0x49,0x49,0x49,0x00,0x3E,0x41,0x41,
	0x41,0x22,0x00,0x7F,0x08,0x14,0x22,0x41,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static unsigned char dmat_fontmap_vertical_CHECKMATE[CHECKMATE_LINE] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, 0x7F, 0x08, 0x08, 0x08,
	0x7F, 0x00, 0x7F, 0x49, 0x49, 0x49, 0x00, 0x3E, 0x41, 0x41,
	0x41, 0x22, 0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, 0x7F,
	0x02, 0x04, 0x08, 0x04, 0x02, 0x7F, 0x00, 0x7E, 0x09, 0x09,
	0x09, 0x7E, 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x7F,
	0x49, 0x49, 0x49, 0x49,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char dmat_fontmap_vertical_STALEMATE[STALEMATE_LINE] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x01, 0x01, 0x7F, 0x01,
	0x01, 0x00, 0x7E, 0x09, 0x09, 0x09, 0x7E, 0x00, 0x7F, 0x40,
	0x40, 0x40, 0x40, 0x00, 0x7F, 0x49, 0x49, 0x49, 0x49, 0x00,
	0x7F, 0x02, 0x04, 0x08, 0x04, 0x02, 0x7F, 0x00, 0x7E, 0x09,
	0x09, 0x09, 0x7E, 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00,
	0x7F, 0x49, 0x49, 0x49, 0x49,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char dmat_fontmap_vertical_WIN[WIN_LINE] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0F, 0x30, 0x40, 0x30, 0x0C, 0x30, 0x40, 0x30, 0x0F, 0x00,
	0x41, 0x41, 0x7F, 0x41, 0x41, 0x00, 0x7F, 0x02, 0x04, 0x08,
	0x10, 0x20, 0x7F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char dmat_fontmap_vertical_LOSE[LOSE_LINE] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x3E, 0x41, 0x41, 0x41,
	0x41, 0x3E, 0x00, 0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x7F,
	0x49, 0x49, 0x49, 0x49,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char dmat_chesspiece[NUM_CHESSPIECE][NUM_SCANLINES] =
{
	{0x00, 0x00, 0x08, 0x1C, 0x14, 0x1C, 0x08, 0x1C, 0x3E, 0x7F},
	{0x00, 0x2A, 0x2A, 0x3E, 0x3E, 0x1C, 0x1C, 0x1C, 0x3E, 0x7F},
	{0x08, 0x14, 0x08, 0x1C, 0x14, 0x1C, 0x08, 0x1C, 0x3E, 0x7F},
	{0x18, 0x1C, 0x3E, 0x3E, 0x7E, 0x0E, 0x0E, 0x1E, 0x3C, 0x7F},
	{0x00, 0x2A, 0x36, 0x1C, 0x08, 0x3E, 0x1C, 0x1C, 0x3E, 0x7F},
	{0x00, 0x08, 0x1C, 0x08, 0x08, 0x3E, 0x1C, 0x1C, 0x3E, 0x7F}
};

// // "CHECK" #48
// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 0x3E,0x41,0x41,0x41,0x22,0x00,0x7F,0x08,0x08,0x08,
// 0x7F,0x00,0x7F,0x49,0x49,0x49,0x00,0x3E,0x41,0x41,
// 0x41,0x22,0x00,0x7F,0x08,0x14,0x22,0x41,
// 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//
// #define CHECK_LINE 48
// #define CHECKMATE_LINE 74
// #define STALEMATE_LINE 75
// #define WIN_LINE 43
// #define LOSE_LINE 44
//
//  // "CHECKMATE" #74
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//  0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, 0x7F, 0x08, 0x08, 0x08,
//  0x7F, 0x00, 0x7F, 0x49, 0x49, 0x49, 0x00, 0x3E, 0x41, 0x41,
//  0x41, 0x22, 0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, 0x7F,
//  0x02, 0x04, 0x08, 0x04, 0x02, 0x7F, 0x00, 0x7E, 0x09, 0x09,
//  0x09, 0x7E, 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x7F,
//  0x49, 0x49, 0x49, 0x49,
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//
// // "STALEMATE" #75
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// 0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x01, 0x01, 0x7F, 0x01,
// 0x01, 0x00, 0x7E, 0x09, 0x09, 0x09, 0x7E, 0x00, 0x7F, 0x40,
// 0x40, 0x40, 0x40, 0x00, 0x7F, 0x49, 0x49, 0x49, 0x49, 0x00,
// 0x7F, 0x02, 0x04, 0x08, 0x04, 0x02, 0x7F, 0x00, 0x7E, 0x09,
// 0x09, 0x09, 0x7E, 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00,
// 0x7F, 0x49, 0x49, 0x49, 0x49,
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// //
// // "WIN" #43
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//  0x0F, 0x30, 0x40, 0x30, 0x0C, 0x30, 0x40, 0x30, 0x0F, 0x00,
//  0x41, 0x41, 0x7F, 0x41, 0x41, 0x00, 0x7F, 0x02, 0x04, 0x08,
//  0x10, 0x20, 0x7F,
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// //
// //  "LOSE" #44
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//  0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x3E, 0x41, 0x41, 0x41,
//  0x41, 0x3E, 0x00, 0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x7F,
//  0x49, 0x49, 0x49, 0x49,
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// //
// //  "PAWN/ROOK/BISHOP/KNIGHT/QUEEN/KING" #
//  0x00, 0x00, 0x08, 0x1C, 0x14, 0x1C, 0x08, 0x1C, 0x3E, 0x7F
//  0x00, 0x2A, 0x2A, 0x3E, 0x3E, 0x1C, 0x1C, 0x1C, 0x3E, 0x7F
//  0x08, 0x14, 0x08, 0x1C, 0x14, 0x1C, 0x08, 0x1C, 0x3E, 0x7F
//  0x18, 0x1C, 0x3E, 0x3E, 0x7E, 0x0E, 0x0E, 0x1E, 0x3C, 0x7F
//  0x00, 0x2A, 0x36, 0x1C, 0x08, 0x3E, 0x1C, 0x1C, 0x3E, 0x7F
//  0x00, 0x08, 0x1C, 0x08, 0x08, 0x3E, 0x1C, 0x1C, 0x3E, 0x7F

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

static void __dmat_write_from_int_2digit(unsigned char *data10, unsigned char *data1)
{
    int i;
    unsigned short out;

    for (i=0; i<NUM_SCANLINES/2; i++)
    {
        out = 0x007F & data10[i];
        iowrite16(out, (dmat_ioremap + i));
    }

    for (i=0; i<NUM_SCANLINES/2; i++)
    {
        out = 0x007F & data1[i];
        iowrite16(out, (dmat_ioremap + i + NUM_SCANLINES/2));
    }
}

static void __dmat_write_from_int_2digit_id(int num)
{
    int i, j;
    int digit;
    int div=10000000, mod;
    unsigned short out;

    for (i=0; i<8; i++)
    {
        digit = num / div;
	num = num % div;
	div /= 10;
        for(j = 0; j < 5; j++)
        {
            dmat_fontmap_vertical_decimal_id[i*5 + j] = dmat_fontmap_vertical_decimal[digit][j];
        }
    }

    for (i=0; i<31; i++)
    {
        for (j=0; j<10; j++)
        {
            out = 0x007F & dmat_fontmap_vertical_decimal_id[i+j];
            iowrite16(out, (dmat_ioremap + j));
        }

        msleep(100);
    }
}

static void __dmat_write_from_name()
{
    int loops;
    int i;
    int j;
    unsigned short out;

    for(loops = 0; loops < NUM_KORNAME-10; loops++){

        for (i=0; i<NUM_SCANLINES; i++)
        {
            out = 0x007F & dmat_fontmap_kor_name[i + loops];
            iowrite16(out, (dmat_ioremap + i));
        }

	msleep(100);
    }
}


/*
Condition details

101 = CHECK
102 = CHECKMATE
103 = STALEMATE
104 = WIN
105 = LOSE
*/
static void __dmat_write_condition(int condition)
{
								int loops;
								int i;
								int j;
								unsigned short out;

								unsigned char* cur_dotmap;

								if(condition == CHECK_CONDITION_VAR)
									cur_dotmap = dmat_fontmap_vertical_CHECK;
								else if(condition == CHECKMATE_CONDITION_VAR)
									cur_dotmap = dmat_fontmap_vertical_CHECKMATE;
								else if(condition == STALEMATE_CONDITION_VAR)
									cur_dotmap = dmat_fontmap_vertical_STALEMATE;
								else if(condition == WIN_CONDITION_VAR)
									cur_dotmap = dmat_fontmap_vertical_WIN;
								else if(condition == LOSE_CONDITION_VAR)
									cur_dotmap = dmat_fontmap_vertical_LOSE;

								int linenum = condition_lineNum[condition-CHECK_CONDITION_VAR];

								for(loops = 0; loops < linenum-9; loops++) {

										for (i=0; i<NUM_SCANLINES; i++)
										{
												out = 0x007F & cur_dotmap[i + loops];
												iowrite16(out, (dmat_ioremap + i));
										}

										msleep(100);
								}
}
/*
type details

0 = PAWN
1 = ROOK
2 = BISHOP
3 = KNIGHT
4 = QUEEN
5 = KING
*/
static void __dmat_write_chesspiece(int type)
{
	int i;
	unsigned short out;

	for (i=0; i<NUM_SCANLINES; i++)
	{
		out = 0x007F & dmat_chesspiece[type][i];
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

	if(num > 0 && num < 7)
		__dmat_write_chesspiece(num-1);
	else if(num > 100 && num < 106)
		__dmat_write_condition(num);
	else
		__dmat_write_from_int(dmat_fontmap_clear);

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
