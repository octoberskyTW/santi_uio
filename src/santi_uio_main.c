#include <linux/module.h>
#include <linux/kernel.h>

static int __init santi_uio_init(void)
{
    printk("Hello World SANTI UIO\n");
    return 0;
}

static void __exit santi_uio_exit(void)
{
    printk("Goodbye, world SANTI UIO\n");
}

module_init(santi_uio_init);
module_exit(santi_uio_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dung-Ru Tsai");