#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uio_driver.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/slab.h>


struct santi_uio_ctrlblk_t {
	void __iomem *base;
    struct uio_info uinfo;
};

static struct santi_uio_ctrlblk_t su_blk = {0};

static int santi_uio_probe(struct platform_device *pdev)
{
    // Device initialization and registration with UIO
	int ret = 0;
    struct device_node *fe_mem = NULL;
    struct santi_uio_ctrlblk_t *su = &su_blk;
    struct uio_info *info = &su->uinfo;
	struct resource res;

	fe_mem = of_parse_phandle(pdev->dev.of_node, "fe_mem", 0);
	if (!fe_mem) {
		pr_err("%s: can not find fe_mem node failed\n", __FUNCTION__);
		return -ENODEV;
	}
	if (of_address_to_resource(fe_mem, 0, &res)) {
        pr_err("%s: of_address_to_resource failed\n", __FUNCTION__);
		return -ENXIO;
    }
	/* map fe_mem */
	su->base = devm_ioremap_resource(&pdev->dev, &res);
	if (IS_ERR(su->base)) {
        pr_err("%s: devm_ioremap_resource failed\n", __FUNCTION__);
        return PTR_ERR(su->base);
    }

	of_node_put(fe_mem);

    info->name = "santi_uio_device";
	info->version = "0.0.1";
	info->mem[0].addr = su->base;
    pr_info("%s: 0x%llx\n", __FUNCTION__, info->mem[0].addr);
	info->mem[0].size = resource_size(&res);
	info->mem[0].memtype = UIO_MEM_PHYS;
    ret = uio_register_device(&pdev->dev, info);
	 if (ret) {
        pr_err("%s: uio_register_device failed: %p\n", __FUNCTION__, ERR_PTR(ret));
		goto err_uio_register_device;
     }
    pr_info("%s: SANTI PROBE P.A.S.S\n", __FUNCTION__);
    return 0;
err_uio_register_device:
	return ret;
}

static int santi_uio_remove(struct platform_device *pdev)
{
    // Cleanup
    struct santi_uio_ctrlblk_t *su = &su_blk;
    struct uio_info *info = &su->uinfo;
    uio_unregister_device(info);
    return 0;
}

// static struct uio_info santi_uio_info = {
//     .name = "SantiUIODriver",
//     .version = "0.1",
//     // Fill in other fields as necessary
// };

static const struct of_device_id of_santi_match[] = {
	{ .compatible = "mediatek,pce", },
	{ },
};

static struct platform_driver santi_uio_driver = {
    .probe = santi_uio_probe,
    .remove = santi_uio_remove,
	.driver = {
		.name = "SantiUIODriver",
		.owner = THIS_MODULE,
		.of_match_table = of_santi_match,
	},
};
MODULE_DEVICE_TABLE(of, of_santi_match);

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