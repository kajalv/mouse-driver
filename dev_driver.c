/*
Linux Device driver module that changes brightness of the screen on click.
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/unistd.h>
#include <linux/buffer_head.h>
#include <asm/segment.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL"); // avoid kernel taint warning
MODULE_DESCRIPTION("Mouse Driver");
MODULE_AUTHOR("Custom");

static int times = 0;
char brightness_buff[10];
int k, btn_left, btn_right, btn_middle;
struct file *filehandle1, *filehandle2;

static int dev_open(struct inode *, struct file *);
static int dev_rls(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/*
Callbacks required to implement a device driver
*/
static struct file_operations fops =
{
	.read = dev_read,
	.open = dev_open,
	.write = dev_write,
	.release = dev_rls,
};

/*
The following functions help in reading/writing to files
within a Linux kernel module. File I/O should be avoided when possible.
Call VFS level functions instead of the syscall handler directly.
*/
struct file* file_open(const char* path, int flags, int rights)
{
	struct file* filp = NULL;
	mm_segment_t oldfs;
	int err = 0;
	oldfs = get_fs();
	set_fs(get_ds());
	filp = filp_open(path, flags, rights);
	set_fs(oldfs);
	if (IS_ERR(filp))
	{
		err = PTR_ERR(filp);
		return NULL;
	}
	return filp;
}

void file_close(struct file* file)
{
	filp_close(file, NULL);
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size)
{
	mm_segment_t oldfs;
	int ret;
	oldfs = get_fs();
	set_fs(get_ds());
	ret = vfs_read(file, data, size, &offset);
	set_fs(oldfs);
	return ret;
}

int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size)
{
	mm_segment_t oldfs;
	int ret;
	oldfs = get_fs();
	set_fs(get_ds());
	ret = vfs_write(file, data, size, &offset);
	set_fs(oldfs);
	return ret;
}

/*
Called when module is loaded
*/
int init_module(void)
{
	// register driver with major number 45 and name mousedev
	// returns a non-negative number on success
	int t = register_chrdev(45, "mousedev", &fops);
	if (t < 0) printk(KERN_ALERT "Device registration failed ...\n");
	else printk(KERN_ALERT "Device registered ...\n");
	return t;
}

/*
Called when module is unloaded
*/
void cleanup_module(void)
{
	unregister_chrdev(45, "mousedev");
}

/*
Called when system call 'open' is done on the device file
*/
static int dev_open(struct inode *inod, struct file *fil)
{
	times++;
	printk(KERN_ALERT "Device opened %d times\n", times);
	return 0;
}

/*
Called when system call 'read' is done on the device file
*/
static ssize_t dev_read(struct file *filp, char *buff, size_t len, loff_t *off)
{
	filehandle1 = file_open("/sys/class/backlight/acpi_video0/brightness", 0, 0);
	file_read((struct file*)filehandle1, 0, brightness_buff, 2);
	filehandle2 = file_open("/dev/input/mice", 0, 0);
	file_read(filehandle2, 0, buff, 1);

	btn_left = buff[0] & 0x1;
	btn_right = buff[0] & 0x2;
	btn_middle = buff[0] & 0x04;

	int brightness =0;
	brightness = (brightness_buff[0]-'0') * 10;
	brightness += (brightness_buff[1]-'0');

	if (btn_left > 0)
	{
		printk("brightness_buff is left click:");
		if (brightness < 95)
			brightness += 5;
		printk(brightness_buff);
	}
	else if (btn_right > 0)
	{
		printk("brightness_buff is right click:");
		if (brightness > 5)
			brightness -= 5;
		printk(brightness_buff);
	}
	else
	{
		printk("brightness_buff is middle click:");
		//do nothing
		printk(brightness_buff);
	}

	brightness_buff[0] = brightness/10 + '0';
	brightness_buff[1] = brightness%10 + '0';

	file_close((struct file*)filehandle1);
	filehandle1 = file_open("/sys/class/backlight/acpi_video0/brightness", 1, 0);
	file_write((struct file*)filehandle1, 0, brightness_buff, 2);
	file_close((struct file*)filehandle1);
	return 0;
}

/*
Called when system call 'write' is done on the device file
*/
static ssize_t dev_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
	return 0;
}

/*
Called when system call 'close' is done on the device file
*/
static int dev_rls(struct inode *inod, struct file *fil)
{
	printk(KERN_ALERT "Device closed.\n");
	return 0;
}
