#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/device.h>


dev_t dev =0 ;
static struct class *dev_class;


static int __init hello_world_init(void)
{
	if((alloc_chrdev_region(&dev, 0, 1, "Manually create file"))<0)
	{
		printk(KERN_INFO "Cannot allocate major number.....\n");
		return -1;
	}
	printk(KERN_INFO "MAJOR = %d MINOR = %d \n ",MAJOR(dev), MINOR(dev));

	 /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"Himanshu_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class for device\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"Himanshu_device")) == NULL){
            printk(KERN_INFO "Cannot create the Device\n");
            goto r_device;
        }
	printk(KERN_INFO "Module initialized successfully................");
	return 0;

	
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

void __exit hello_world_exit(void)
{
	device_destroy(dev_class,dev);
        class_destroy(dev_class);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Module remove successfully...........\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("HIMANSHU");
MODULE_DESCRIPTION("A simple hello world program");
MODULE_VERSION("1.1");
