#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/module.h>
#include<linux/init.h>


dev_t dev=0;	//Major number init


static int __init hello_world_init(void)
{
	if((alloc_chrdev_region(&dev, 0,1, "Major_minor"))<0)		//Dynamically allocation of major and minor number
	{
		printk(KERN_INFO "Cannot allocate amjor number for device \n");
		return -1;
	}
	printk(KERN_INFO "MAJOR= %d MINOR=%d\n",MAJOR(dev), MINOR(dev));
	printk(KERN_INFO "Kernel module initalized succesfully.........");
	return 0;

}

void __exit hello_world_exit(void)
{
	unregister_chrdev_region(dev,1);			//Unregister character driver
	printk(KERN_INFO "Kernel module exit successfully...........");
}

/******************************Module Functions*******************************/
module_init(hello_world_init);
module_exit(hello_world_exit);

/*******************************Module Authentication********************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Himanshu Gupta");
MODULE_DESCRIPTION("A simple hello world driver");
MODULE_VERSION("1.1");
