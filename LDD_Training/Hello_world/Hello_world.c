#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>


static int __init hello_world_init(void)
{
	int value =0;
	printk(KERN_INFO "value %d \n", value);
        printk(KERN_INFO "This is the Simple Module\n");
        printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
	return 0;
}

void __exit hello_world_exit(void)
{
	printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Himanshu Gupta");
MODULE_DESCRIPTION("A simple hello world driver");
MODULE_VERSION("2:1.0");
