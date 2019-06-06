#include <linux/kernel.h>	//for kernel init
#include <linux/init.h>		//Init
#include <linux/module.h>	//Module init
#include <linux/kdev_t.h>	//MAJOR AND MINOR Functions
#include <linux/fs.h>		//For Character driver
#include <linux/cdev.h>		//cdev structure
#include <linux/device.h>	//device driver
 

dev_t dev = 0;			//Major minor init
static struct class *dev_class;	//class init
static struct cdev hello_cdev;	//structure
 
static int __init hello_driver_init(void);
static void __exit hello_driver_exit(void);
static int hello_open(struct inode *inode, struct file *file);
static int hello_release(struct inode *inode, struct file *file);
static ssize_t hello_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t hello_write(struct file *filp, const char *buf, size_t len, loff_t * off);
 

/******************************FILE OPERATION STRUCTURE********************************/
static struct file_operations fops =
{
.owner          = THIS_MODULE,
.read           = hello_read,
.write          = hello_write,
.open           = hello_open,
.release        = hello_release,
};
 

/*************************************File operation functions***************************/
static int hello_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Driver Open Function Called...!!!\n");
        return 0;
}
 
static int hello_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Driver Release Function Called...!!!\n");
        return 0;
}
 
static ssize_t hello_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Driver Read Function Called...!!!\n");
        return 0;
}
static ssize_t hello_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Driver Write Function Called...!!!\n");
        return len;
}
 
 /***********************************Driver Init**************************************/
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
 

/**************************************Driver removal*****************************/
void __exit hello_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&hello_cdev);
        unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 

/***************************Module Function**********************************/
module_init(hello_driver_init);
module_exit(hello_driver_exit);
 

/*****************************Module Authentication********************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Himanshu Gupta");
MODULE_DESCRIPTION("A simple device driver");
MODULE_VERSION("1.3");
