#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/device.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/sysfs.h>
#include<linux/kobject.h>
#include<linux/interrupt.h>
#include<asm/io.h>
#include<linux/workqueue.h>

#define IRQ_NO 11



/* Work structure */

static struct workqueue_struct *own_workqueue;

static void workqueue_fn(struct work_struct *work);

static DECLARE_WORK(work, workqueue_fn);

//static struct work_struct workqueue;

//void workqueue_fn(struct work_struct *work);

static void workqueue_fn(struct work_struct *work){

	printk(KERN_INFO "EXECUTING WORK FUNCTION SUCCESSFULLY..............!!!!\n");
	return;
}



//Interrupt handler for IRQ_NO 11

static irqreturn_t irq_handler(int irq, void *dev_id){
	printk(KERN_INFO "SHARED IRQ : INTERRUPT OCCURED");
	//schedule_work(&workqueue);
	queue_work(own_workqueue, &work);
	return IRQ_HANDLED;
}


volatile int hello_value=0;

dev_t dev =0;
static struct class *dev_class;
static struct cdev hello_cdev;
struct kobject *kobj_ref;

static int __init hello_driver_init(void);
static void __exit hello_driver_exit(void);



/******************************DRIVER FUNCTION**********************************************/

static int hello_open(struct inode *inode, struct file *file);
static int hello_release(struct inode *inode, struct file *file);
static ssize_t hello_read(struct file *filp, char __user *buf, size_t len, loff_t * off);
static ssize_t hello_write(struct file *filp, const char *buf, size_t len, loff_t * off);


/*******************************SYSFS FUNCTION**********************************************/

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

struct kobj_attribute hello_attr= __ATTR(hello_value, 0660, sysfs_show, sysfs_store);


static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = hello_read,
        .write          = hello_write,
        .open           = hello_open,
        .release        = hello_release,
};
 

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf){

	printk(KERN_INFO "SYSFS READ SUCCESSFULLY.................!!!\n");
	return sprintf(buf, "%d", hello_value);
}

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){

	printk(KERN_INFO "SYSFS WRITE SUCCESSFULLY...................!!!\n");
	sscanf(buf, "%d", &hello_value);
	return count;
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
 
static ssize_t hello_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Read function\n");
        asm("int $0x3B");  // Corresponding to irq 11
        return 0;
}
static ssize_t hello_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Write Function\n");
        return 0;
}
 

static int __init hello_driver_init(void){

	/*ALLOCATING MAJOR NUMBER*/
	
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
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
 
        /*Creating a directory in /sys/kernel/ */
        kobj_ref = kobject_create_and_add("hello_sysfs",kernel_kobj);
 
        /*Creating sysfs file for hello_value*/
        if(sysfs_create_file(kobj_ref,&hello_attr.attr)){
                printk(KERN_INFO"Cannot create sysfs file......\n");
                goto r_sysfs;
        }
        if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "hello_device", (void *)(irq_handler))) {
            printk(KERN_INFO "my_device: cannot register IRQ ");
                    goto irq;
        }
	
	/*Creating workqueue */
        own_workqueue = create_workqueue("own_wq");

        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
    return 0;
 
irq:
        free_irq(IRQ_NO,(void *)(irq_handler));
 
r_sysfs:
        kobject_put(kobj_ref); 
        sysfs_remove_file(kernel_kobj, &hello_attr.attr);
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        cdev_del(&hello_cdev);
        return -1;
}
 
void __exit hello_driver_exit(void)
{
        /* Delete workqueue */
        destroy_workqueue(own_workqueue);
	free_irq(IRQ_NO,(void *)(irq_handler));
        kobject_put(kobj_ref); 
        sysfs_remove_file(kernel_kobj, &hello_attr.attr);
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
MODULE_DESCRIPTION("A simple device driver -OWN WORKQUEUE");
MODULE_VERSION("1.12");

