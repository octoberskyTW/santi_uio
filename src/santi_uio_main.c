#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uio_driver.h>
#include "santi_uio_main.h"


#if 0
//static struct santi_uio_ctrlblk_t su_blk = {0};

static irqreturn_t santi_uio_irqhandler(int irq, struct uio_info *dev_info)
{
	struct santi_uio_ctrlblk_t *priv = dev_info->priv;

	/* Just disable the interrupt in the interrupt controller, and
	 * remember the state so we can allow user space to enable it later.
	 */

	spin_lock(&priv->lock);
	if (!test_and_set_bit(0, &priv->flags))
		disable_irq_nosync(irq);
	spin_unlock(&priv->lock);

	return IRQ_HANDLED;
}
#endif

static int santi_uio_probe(struct platform_device *pdev)
{
    // Device initialization and registration with UIO
    int ret = 0;
    struct device_node *fe_mem = NULL;
    struct santi_uio_ctrlblk_t *su_ctrl = NULL;
    struct uio_info *info = NULL;
    struct resource res;

    pr_info("Entering %s\n", __func__);

    /*allocate and init santi_uio_ctrlblk (su_ctrl)*/
    su_ctrl = devm_kmalloc(&pdev->dev, sizeof(*su_ctrl), GFP_KERNEL);
    if (!su_ctrl)
        return -ENOMEM;

    memset(su_ctrl, 0, sizeof(*su_ctrl));

    /*get memory info from device tree*/
    fe_mem = of_parse_phandle(pdev->dev.of_node, "fe_mem", 0);
    if (!fe_mem) {
        pr_err("%s: can not find fe_mem node failed\n", __FUNCTION__);
        return -ENODEV;
    }

    if (of_address_to_resource(fe_mem, 0, &res)) {
        pr_err("%s: of_address_to_resource failed\n", __FUNCTION__);
        return -ENXIO;
    }
    pr_info("%s: res.start=0x%llx, res.end=0x%llx, res_size=0x%llx \n", __FUNCTION__, res.start, res.end, resource_size(&res));

    /* Mapping the Virtual Memory */
    su_ctrl->base = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
    if (IS_ERR(su_ctrl->base)) {
        pr_err("%s: devm_ioremap failed\n", __FUNCTION__);
        return PTR_ERR(su_ctrl->base);
    }

    of_node_put(fe_mem);

    /*prepare struct uio_info*/
    info = &su_ctrl->uinfo;

    info->name = "santi_uio_device";
    info->version = "0.0.1";
    info->mem[0].internal_addr = su_ctrl->base;
    pr_info("%s: Virtual Physical Address 0x%llx\n", __FUNCTION__, info->mem[0].internal_addr);
    info->mem[0].addr = res.start;
    pr_info("%s: Physical Address 0x%llx\n", __FUNCTION__, info->mem[0].addr);
    info->mem[0].size = resource_size(&res);
    info->mem[0].memtype = UIO_MEM_PHYS;

    ret = uio_register_device(&pdev->dev, info);
    if (ret) {
        devm_iounmap(&pdev->dev, info->mem[0].internal_addr);
        pr_err("%s: uio_register_device failed: %p\n", __FUNCTION__,
               ERR_PTR(ret));
        goto err_uio_register_device;
    }

    platform_set_drvdata(pdev, su_ctrl);
    pr_info("%s: SANTI PROBE P.A.S.S\n", __FUNCTION__);
    return 0;
err_uio_register_device:
    return ret;
}

static int santi_uio_remove(struct platform_device *pdev)
{
    // Cleanup
    struct santi_uio_ctrlblk_t *su_ctrl = platform_get_drvdata(pdev);
    struct uio_info *info = &su_ctrl->uinfo;
    uio_unregister_device(info);
    return 0;
}

static const struct of_device_id of_santi_match[] = {
    {
        .compatible = "mediatek,santi_uio",
    },
    {},
};

static struct platform_driver santi_uio_driver = {
    .probe = santi_uio_probe,
    .remove = santi_uio_remove,
    .driver =
        {
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