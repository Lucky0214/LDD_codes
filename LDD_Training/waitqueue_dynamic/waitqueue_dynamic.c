#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()

#include <linux/kthread.h>
#include <linux/wait.h>                // Required for the wait queues


uint32_t read_count = 0;
static struct task_struct *wait_thread;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev hello_cdev;
wait_queue_head_t wait_queue_hello;
int wait_queue_flag = 0;

static int __init hello_driver_init(void);
static void __exit hello_driver_exit(void);

/*************** Driver Fuctions **********************/
static int hello_open(struct inode *inode, struct file *file);
static int hello_release(struct inode *inode, struct file *file);
static ssize_t hello_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t hello_write(struct file *filp, const char *buf, size_t len, loff_t * off);


static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = hello_read,
        .write          = hello_write,
        .open           = hello_open,
        .release        = hello_release,
};


static int wait_function(void *unused)
{
        
        while(1) {
                printk(KERN_INFO "Waiting For Event...\n");
                wait_event_interruptible(wait_queue_hello, wait_queue_flag != 0 );
                if(wait_queue_flag == 2) {
                        printk(KERN_INFO "Event Came From Exit Function\n");
                        return 0;
                }
                printk(KERN_INFO "Event Came From Read Function - %d\n", ++read_count);
                wait_queue_flag = 0;
        }
        return 0;
}



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
        wait_queue_flag = 1;
        wake_up_interruptible(&wait_queue_hello);
        return 0;
}
static ssize_t hello_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Write function\n");
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
        
        //Initialize wait queue
        init_waitqueue_head(&wait_queue_hello);

        //Create the kernel thread with name 'mythread'
        wait_thread = kthread_create(wait_function, NULL, "WaitThread");
        if (wait_thread) {
                printk("Thread Created successfully\n");
                wake_up_process(wait_thread);
        } else
                printk(KERN_INFO "Thread creation failed\n");

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
        wait_queue_flag = 2;
        wake_up_interruptible(&wait_queue_hello);
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
MODULE_VERSION("1.7");
