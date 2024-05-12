#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/uio_driver.h>
#include "santi_uio_main.h"


#define FE_INT_STATUS 0x08
#define FE_INT_ENABLE 0x0c

enum mtk_fe_event_id {
	MTK_EVENT_FORCE		= 0,
	MTK_EVENT_WARM_CNT	= 1,
	MTK_EVENT_COLD_CNT	= 2,
	MTK_EVENT_TOTAL_CNT	= 3,
	MTK_EVENT_FQ_EMPTY	= 8,
	MTK_EVENT_TSO_FAIL	= 12,
	MTK_EVENT_TSO_ILLEGAL	= 13,
	MTK_EVENT_TSO_ALIGN	= 14,
	MTK_EVENT_RFIFO_OV	= 18,
	MTK_EVENT_RFIFO_UF	= 19,
    MTK_MAC1_LINK = 24,
    MTK_MAC2_LINK = 25,
};

char* mtk_fe_event_name[32] = {
	[MTK_EVENT_FORCE]	= "Force",
	[MTK_EVENT_WARM_CNT]	= "Warm",
	[MTK_EVENT_COLD_CNT]	= "Cold",
	[MTK_EVENT_TOTAL_CNT]	= "Total",
	[MTK_EVENT_FQ_EMPTY]	= "FQ Empty",
	[MTK_EVENT_TSO_FAIL]	= "TSO Fail",
	[MTK_EVENT_TSO_ILLEGAL]	= "TSO Illegal",
	[MTK_EVENT_TSO_ALIGN]	= "TSO Align",
	[MTK_EVENT_RFIFO_OV]	= "RFIFO OV",
	[MTK_EVENT_RFIFO_UF]	= "RFIFO UF",
    [MTK_MAC1_LINK]	= "MAC1 LINK STAT CHANGE",
    [MTK_MAC2_LINK]	= "MAC2 LINK STAT CHANGE",
};


void santi_uio_w32(struct santi_uio_ctrlblk_t *su_ctrl, u32 val, unsigned reg)
{
	__raw_writel(val, su_ctrl->base + reg);
}

u32 santi_uio_r32(struct santi_uio_ctrlblk_t *su_ctrl, unsigned reg)
{
	return __raw_readl(su_ctrl->base + reg);
}

static irqreturn_t fe_irqhandler(int irq, struct uio_info *dev_info)
{
	struct santi_uio_ctrlblk_t *su_ctrl = dev_info->priv;
	u32 status = 0, val = 0;

    status = santi_uio_r32(su_ctrl, FE_INT_STATUS);
    pr_info("[%s] Trigger FE Misc ISR: 0x%x\n", __func__, status);

	while (status) {
		val = ffs((unsigned int)status) - 1;
		status &= ~(1 << val);

		if ((val == MTK_EVENT_TSO_FAIL) ||
		    (val == MTK_EVENT_TSO_ILLEGAL) ||
		    (val == MTK_EVENT_TSO_ALIGN) ||
		    (val == MTK_EVENT_RFIFO_OV) ||
		    (val == MTK_EVENT_RFIFO_UF) ||
            (val == MTK_MAC1_LINK) ||
            (val == MTK_MAC2_LINK))
			pr_info("[%s] Detect FE event: %s !\n", __func__,
				mtk_fe_event_name[val]);
	}
    /*reset interrupt status CR*/
    santi_uio_w32(su_ctrl, 0xFFFFFFFF, FE_INT_STATUS);
	return IRQ_HANDLED;
}

static int santi_uio_probe(struct platform_device *pdev)
{
    // Device initialization and registration with UIO
    int ret = 0;
    struct device_node *fe_mem = NULL;
    struct santi_uio_ctrlblk_t *su_ctrl = NULL;
    struct uio_info *info = NULL;
    struct resource res;
    int err;

    pr_info("Entering %s\n", __func__);

    /*allocate and init santi_uio_ctrlblk (su_ctrl)*/
    su_ctrl = devm_kmalloc(&pdev->dev, sizeof(*su_ctrl), GFP_KERNEL);
    if (!su_ctrl)
        return -ENOMEM;

    memset(su_ctrl, 0, sizeof(*su_ctrl));
    su_ctrl->dev = &pdev->dev;

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

    /* Mapping the Virtual Memory */
    su_ctrl->base = devm_ioremap(&pdev->dev, res.start, resource_size(&res));
    if (IS_ERR(su_ctrl->base)) {
        pr_err("%s: devm_ioremap failed\n", __FUNCTION__);
        return PTR_ERR(su_ctrl->base);
    }

    of_node_put(fe_mem);

    /*prepare struct uio_info*/
    info = &su_ctrl->uinfo;
    info->priv = su_ctrl;

    info->name = "santi_uio_device";
    info->version = "0.0.1";
    info->mem[0].internal_addr = su_ctrl->base;
    info->mem[0].addr = res.start;
    info->mem[0].size = resource_size(&res);
    info->mem[0].memtype = UIO_MEM_PHYS;

    /*Setup FE Interrupt*/
    su_ctrl->fe_irq = platform_get_irq(pdev, 0);
    if (su_ctrl->fe_irq < 0) {
        pr_err("%s: Can't get IRQ%d resource.\n", __FUNCTION__, 0);
        goto err_uio_register_device;
    }

    info->irq = su_ctrl->fe_irq;
    info->irq_flags = IRQF_TRIGGER_NONE;
    info->handler = fe_irqhandler;

    /*Enable MAC Link INT for test*/
    santi_uio_w32(su_ctrl, 0x030C7000, FE_INT_ENABLE);


    ret = uio_register_device(&pdev->dev, info);
    if (ret) {
        pr_err("%s: uio_register_device failed: %p\n", __FUNCTION__,
               ERR_PTR(ret));
        goto err_uio_register_device;
    }

    platform_set_drvdata(pdev, su_ctrl);
    pr_info("%s: SANTI PROBE P.A.S.S\n", __FUNCTION__);
    return 0;
err_uio_register_device:
    if(info->mem[0].internal_addr)

        devm_iounmap(&pdev->dev, info->mem[0].internal_addr);
    pr_info("%s: SANTI PROBE F.A.I.L\n", __FUNCTION__);
    return ret;
}

static int santi_uio_remove(struct platform_device *pdev)
{
    // Cleanup
    struct santi_uio_ctrlblk_t *su_ctrl = platform_get_drvdata(pdev);
    struct uio_info *info = &su_ctrl->uinfo;
    if (info)
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