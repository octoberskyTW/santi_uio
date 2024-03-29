# SPDX-Liscense-Identifier: GPL-2.0-or-later
#
# Copyright (C) 2024 Dung-Ru Tsai
#
# Author: Dungru Tsai <dungru.tw@gmail.com>
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=santi_uio
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/package-defaults.mk

# For package initialization such as menuconfig or description etc.
UIO_KERNEL_PKGS:=
UIO_PKGS:=

UIO_KERNEL_PKGS+= \
	santi_uio

ifeq ($(CONFIG_PACKAGE_kmod-santi_uio),y)
EXTRA_KCONFIG+= \
	CONFIG_SANTI_UIO=$(CONFIG_SANTI_UIO)
endif

define KernelPackage/santi_uio
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  TITLE:=kernel santi_uio
  VERSION:=$(LINUX_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:= $(PKG_BUILD_DIR)/santi_uio.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,46,santi_uio)
endef

define KernelPackage/santi_uio/description
	SANTI Userspace IO Mode
endef

EXTRA_CFLAGS+= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG))))

EXTRA_CFLAGS+= -Wall -Werror

define Build/Prepare
# $(CP) -r `find ./src -maxdepth 1 | grep -v ".git"` $(PKG_BUILD_DIR)/
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		$(EXTRA_KCONFIG) \
		modules
endef

define KernelPackage/santi_uio/install
	$(INSTALL_DIR) $(1)/lib/firmware/
endef

$(foreach KERNEL_PKG,$(UIO_KERNEL_PKGS),$(eval $(call KernelPackage,$(KERNEL_PKG))))
$(foreach PKG,$(UIO_PKGS),$(eval $(call BuildPackage,$(PKG))))
