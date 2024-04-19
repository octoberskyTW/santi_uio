#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uio_driver.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
static struct uio_info *g_info;

static int santi_uio_probe(struct platform_device *pdev)
{
    // Device initialization and registration with UIO
    struct uio_info *info;
    info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
    if (!info)
        return -ENOMEM;
    g_info = info;
    info->name = "my_uio_device";
	info->version = "0.0.1";
	 if (uio_register_device(&pdev->dev, info))
		goto out_free;
out_free:
	kfree (info);
	return -ENODEV;
}

static int santi_uio_remove(struct platform_device *pdev)
{
    // Cleanup
    struct uio_info *info = g_info;
    uio_unregister_device(info);
    return 0;
}

// static struct uio_info santi_uio_info = {
//     .name = "SantiUIODriver",
//     .version = "0.1",
//     // Fill in other fields as necessary
// };

static struct platform_driver santi_uio_driver = {
    .probe = santi_uio_probe,
    .remove = santi_uio_remove,
    .driver = {
        .name = "SantiUIODriver",
        // Other fields as necessary
    },
};

static int __init santi_uio_init(void)
{
    int ret;
    pr_info("SANTI_UIO init\n");
    ret = platform_driver_register(&santi_uio_driver);
    if (ret) {
        pr_err("Unable to register SANTI_UIO driver\n");
        return ret;
    }
    return 0;
}

static void __exit santi_uio_exit(void)
{
    platform_driver_unregister(&santi_uio_driver);
    pr_info("SANTI_UIO exit\n");
}


module_init(santi_uio_init);
module_exit(santi_uio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dung-Ru Tsai");