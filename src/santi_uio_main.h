
#include <linux/uio_driver.h>

struct santi_uio_ctrlblk_t {
    void __iomem *base;
    struct uio_info uinfo;
};
