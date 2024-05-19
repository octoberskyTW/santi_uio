# All rights reserved.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=santi_uio
PKG_FILE_DEPENDS := $(SRC_DIR)
include $(INCLUDE_DIR)/package.mk

define KernelPackage/santi_uio
	CATEGORY:=Kernel modules
	SUBMENU:=Other modules
	KCONFIG:=CONFIG_UIO=y \
	  CONFIG_UIO_CIF=n \
	  CONFIG_UIO_PDRV_GENIRQ=n \
	  CONFIG_UIO_DMEM_GENIRQ=n \
	  CONFIG_UIO_AEC=n \
	  CONFIG_UIO_SERCOS3=n \
	  CONFIG_UIO_PCI_GENERIC=n \
	  CONFIG_UIO_NETX=n \
	  CONFIG_UIO_PRUSS=n \
	  CONFIG_UIO_MF624=n
	TITLE:=kernel santi_uio
	FILES:=$(PKG_BUILD_DIR)/santi_uio.ko
	AUTOLOAD:=$(call AutoLoad,46,santi_uio)
	MENU:=1
endef

EXTRA_KCONFIG:= \
	CONFIG_SANTI_UIO=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp -rf ./src/* $(PKG_BUILD_DIR)
endef

MAKE_OPTS:= \
	$(KERNEL_MAKE_FLAGS) \
	M="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

define KernelPackage/santi_uio/install
	$(INSTALL_DIR) $(1)/lib/modules/$(LINUX_VERSION)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/santi_uio.ko $(1)/lib/modules/$(LINUX_VERSION)
endef

$(eval $(call KernelPackage,santi_uio))

