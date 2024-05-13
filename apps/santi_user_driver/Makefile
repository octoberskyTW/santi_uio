#
# Santi_UIO Userspace Drover
#

include $(TOPDIR)/rules.mk

PKG_NAME:=santi_uio_driver
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_CONFIG_DEPENDS:=


include $(INCLUDE_DIR)/package.mk

define Package/santi_uio_driver
  CATEGORY:=Development
  TITLE:=Userspace Driver of santi_uio devices
  DEPENDS:=
  MENU:=1
endef

define Package/santi_uio_driver/description
  Userspace driver for santi_uio
endef

TARGET_CFLAGS += -Wall
TARGET_CFLAGS += -Wno-error=format-security -ggdb3

define Package/santi_uio_driver/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/santi_uio_driver $(1)/usr/bin
endef

$(eval $(call BuildPackage,santi_uio_driver))
