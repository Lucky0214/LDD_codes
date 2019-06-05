#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>

dev_t dev =0 ;

static int __init hello_world_init(void)
{
	if((alloc_chrdev_region(&dev, 0, 1, "Manually create file"))<0)
	{
		printk(KERN_INFO "Cannot allocate major number.....\n");
		return -1;
	}
	printk(KERN_INFO "MAJOR = %d MINOR = %d \n ",MAJOR(dev), MINOR(dev));
	printk(KERN_INFO "Module initialized successfully................");
	return 0;
}

void __exit hello_world_exit(void)
{
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Module remove successfully...........\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("HIMANSHU");
MODULE_DESCRIPTION("A simple hello world program");
MODULE_VERSION("1.1");
