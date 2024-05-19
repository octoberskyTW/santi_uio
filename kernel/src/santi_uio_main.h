
#include <linux/uio_driver.h>

struct santi_uio_ctrlblk_t {
    struct device *dev;
    void __iomem *base;
    struct uio_info uinfo;
    spinlock_t	tx_irq_lock;
    spinlock_t	rx_irq_lock;
    int fe_irq;
};
