#
# Test tool for UIO device
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uioctl
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_CONFIG_DEPENDS:=


include $(INCLUDE_DIR)/package.mk

define Package/uioctl
  CATEGORY:=Development
  TITLE:=Read/Write UIO device
  DEPENDS:=
  MENU:=1
endef

define Package/uioctl/description
  Read/Write UIO devices
endef

TARGET_CFLAGS += -Wall
TARGET_CFLAGS += -Wno-error=format-security -ggdb3

define Package/uioctl/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uioctl $(1)/usr/bin
endef

$(eval $(call BuildPackage,uioctl))
