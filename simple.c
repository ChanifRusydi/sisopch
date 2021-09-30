#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>

int simple_init(void){
    printk(KERN_INFO "Loading  Module\n");
    return 0;
}
void simple_exit(void){
    printk(KERN_INFO "removing Module \n");
}
module_init(simple_init);
modulE_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");