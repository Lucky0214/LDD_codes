#include<linux/kernel.h>	//For kernel
#include<linux/init.h>		// Init
#include<linux/module.h>	//Module
#include<linux/fs.h>		//Character driver
#include<linux/kdev_t.h>	
#include<linux/cdev.h>
#include<linux/slab.h>		//kmalloc()
#include<linux/device.h>	
#include<linux/uaccess.h>	//copy_to/from_user
#include<linux/ioctl.h>		//input_output_control_function

#define WR_VALUE _IOW('a' , 'a', int32_t*)
#define RD_VALUE _IOR('a' , 'b', int32_t*)

int32_t value =0;

dev_t dev=0;
static struct class *dev_class;
static struct cdev hello_cdev;
 
static int __init hello_driver_init(void);
static void __exit hello_driver_exit(void);
static int hello_open(struct inode *inode, struct file *file);
static int hello_release(struct inode *inode, struct file *file);
static ssize_t hello_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t hello_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long hello_ioctl(struct file *file, unsigned int cmd, unsigned long arg);



static struct file_operations fops =
{
	.owner		=THIS_MODULE,
        .read           = hello_read,
        .write          = hello_write,
        .open           = hello_open,
        .unlocked_ioctl = hello_ioctl,
        .release        = hello_release,
};

static int hello_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Opened...!!!\n");
        return 0;
}
 
static int hello_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Closed...!!!\n");
        return 0;
}
 
static ssize_t hello_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Read Function\n");
        return 0;
}
static ssize_t hello_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Write function\n");
        return 0;
}



static long hello_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
		case WR_VALUE:
			copy_from_user(&value, (int32_t*) arg, sizeof(value));
			printk(KERN_INFO "value = %d \n", value);
			break;
		case RD_VALUE:
			copy_to_user((int32_t*) arg, &value, sizeof(value));
			break;
	}
	return 0;
}


static int __init hello_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "hello_Dev")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&hello_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&hello_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"hello_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"hello_device")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto r_device;
        }
        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
    return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
 
void __exit hello_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&hello_cdev);
        unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 
module_init(hello_driver_init);
module_exit(hello_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Himanshu Gupta");
MODULE_DESCRIPTION("A simple device driver");
MODULE_VERSION("1.5");
	


