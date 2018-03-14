#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=serial_read_save
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_BUILD_DIR:= $(BUILD_DIR)/$(PKG_NAME)
PKG_MAINTAINER:=yh
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define Package/serial_read_save    
	SECTION:=utils  
	CATEGORY:=Utilities  
	TITLE:=serial_read_save  -- a serial read and save Program
	DEPENDS:+=libc 
	DEPENDS:+=libm 
	MAINTAINER:=yh  
endef

define Package/serial_read_save/description
	a serial read and save Program
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef
 
define Package/serial_read_save/install
	$(INSTALL_DIR) $(1)/bin
	$(CP) /lib/libc.so.6 $(1)/bin
	$(CP) /lib/libm.so.6 $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/serial_read_save $(1)/bin/

endef
 
$(eval $(call BuildPackage,serial_read_save))
