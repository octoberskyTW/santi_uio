# SANTI_UIO


## Replace DTS file

### Modify the target/linux/mediatek/image/mt7988.mk
```
define Device/santi-dsa-10g-spim-nand
  DEVICE_VENDOR := santi
  DEVICE_MODEL := santi-dsa-10g-spim-nand
  DEVICE_DTS := santi-dsa-10g-spim-nand
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := santi-dsa-10g-spim-snand
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += santi-dsa-10g-spim-nand

define Device/santi-gsw-10g-spim-nand
  DEVICE_VENDOR := santi
  DEVICE_MODEL := santi-gsw-10g-spim-nand
  DEVICE_DTS := santi-gsw-10g-spim-nand
  DEVICE_DTS_DIR := $(DTS_DIR)/mediatek
  SUPPORTED_DEVICES := santi-gsw-10g-spim-snand
  UBINIZE_OPTS := -E 5
  BLOCKSIZE := 128k
  PAGESIZE := 2048
  IMAGE_SIZE := 65536k
  KERNEL_IN_UBI := 1
  IMAGES += factory.bin
  IMAGE/factory.bin := append-ubi | check-size $$$$(IMAGE_SIZE)
  IMAGE/sysupgrade.bin := sysupgrade-tar | append-metadata
endef
TARGET_DEVICES += santi-gsw-10g-spim-nand

```
### Put the DTS into build_dir
```
cp dts_files/*.dts* openwrt/build_dir/target-aarch64_cortex-a53_musl/linux-mediatek_mt7988/linux-5.4.260/scripts/dtc/include-prefixes/arm64/mediatek/
```
## Coding Style
Use clang-format to fix the coding style
```
cd santi_uio
clang-format -i src/*.c
```
## tools

### uioctl
uioctl is used to test memory R/W UIO device(/dev/uioX) in filesystem
```
make menuconfig //select uioctl under Development catagory
make package/kernel/santi_uio/tools/uioctl/compile V=s
```
## apps

### santi_uio_driver
santi_uio_driver is an usersapce driver that deals with the events from santi_uio.ko kernel driver.
```
make menuconfig //select santi_uio_driver under Development catagory
make package/kernel/santi_uio/app/santi_uio_driver/{clean,compile} V=s
```
