#include<linux/kernel.h>	
#include<linux/init.h>		//Kernel Init	
#include<linux/module.h>	// Module Init
#include <linux/fs.h>		//Character device driver
 

/**********************MKDEV function initialized major number and minor number********************/
dev_t dev = MKDEV(235, 0);


static int __init hello_world_init(void)
{
    register_chrdev_region(dev, 1, "Major_Minor");	//Statically registered character driver
    printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    return 0;
}
 
void __exit hello_world_exit(void)
{
    unregister_chrdev_region(dev, 1); 			//Unregister character driver
    printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}


 
/*********************Module Functions*****************************************/
module_init(hello_world_init);
module_exit(hello_world_exit);
 


/**********************************MODULE AUTHENCATION**************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Himanshu Gupta");
MODULE_DESCRIPTION("A simple hello world driver");
MODULE_VERSION("1.0");
