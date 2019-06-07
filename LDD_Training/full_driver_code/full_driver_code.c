#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include <linux/cdev.h>			
#include <linux/device.h>		//Device Driver
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>		//copy_to/from_user


#define mem_size 1024

dev_t dev=0;
static struct class *dev_class;
static struct cdev hello_cdev;
uint8_t *kernel_buffer;

static int __init hello_world_init(void);
static void __exit hello_world_exit(void);
static int hello_open(struct inode *inode, struct file *file);
static int hello_release(struct inode *inode, struct file *file);
static ssize_t hello_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t hello_write(struct file *filp, const char *buf, size_t len, loff_t * off);

static struct file_operations fops =
{
	.owner		= THIS_MODULE,
        .read           = hello_read,
        .write          = hello_write,
        .open           = hello_open,
        .release        = hello_release,
};


static int hello_open(struct inode *inode, struct file *file)
{
	/*Dynamically memory allocation*/
	if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0)
	{
		printk(KERN_INFO "CANNOT ALLOCATE MEMORY IN KERNEL");
		return -1;
	}
	printk(KERN_INFO "DEVICE FILE OPEN....");
	return 0;
}

static int hello_release(struct inode *inode, struct file *file)
{
	kfree(kernel_buffer);
	printk(KERN_INFO "DEVICE FILE CLOSED....");
	return 0;
}

static ssize_t hello_read(struct file *fiip, char __user *buf, size_t len, loff_t * off)
{
	copy_to_user(buf, kernel_buffer, mem_size);
	printk(KERN_INFO "DATA READ DONE....!");
	return mem_size;
}

static ssize_t hello_write(struct file *filp, const char __user *buf, size_t len, loff_t * off)
{
	copy_from_user(kernel_buffer, buf, len);
	printk(KERN_INFO " DATA WRITE DONE....!");
	return len;
}

static int __init hello_world_init(void)
{
	/*ALLOCATING MAJOR NUMBER*/
	if((alloc_chrdev_region(&dev, 0, 1, "hello_dev"))<0)
	{
		printk(KERN_INFO "CANNOT ALLOCATE MAJOR NUMBER");
		return -1;
	}
	printk(KERN_INFO "MAJOR=%d MINOR=%d \n", MAJOR(dev), MINOR(dev));

	/*CREATING CDEV STRUCTURE*/
	cdev_init(&hello_cdev, &fops);
	
	/*ADDING CHARACTER DEVICE TO THE SYSTEM*/
	if((cdev_add(&hello_cdev, dev, 1)) < 0)
	{
		printk(KERN_INFO "CAN NOT ADD A DEVICE TO THE SYSTEM");
		goto r_class;
	}
	
	/*CREATING STRUCTURE CLASS*/
	if((dev_class = class_create(THIS_MODULE, "hello_class")) == NULL)
	{
		printk(KERN_INFO "CAN NOT CREATE THE STRUCTURE CLASS");
                goto r_class;
	}
	
	/*CREATING DEVICE*/
	if((device_create(dev_class, NULL, dev, NULL, "hello_device")) == NULL)
	{
		printk(KERN_INFO "CAN NOT CREATE DEVICE");
                goto r_device;
	}
	printk(KERN_INFO "DEVICE DRIVER INSERT ..... SUCCESSFULLY!");
	return 0;
r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
}
 

void __exit hello_world_exit(void)
{
	device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&hello_cdev);
        unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}


module_init(hello_world_init);
module_exit(hello_world_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Himanshu Gupta");
MODULE_DESCRIPTION("A simple device driver");
MODULE_VERSION("1.4");
