#******************************************************************************#
#*                                                                            *#
#* src/Makefile                                                               *#
#*                                                                 2019/07/01 *#
#* Copyright (C) 2019 Mochi                                                   *#
#*                                                                            *#
#******************************************************************************#
#******************************************************************************#
#* 設定                                                                       *#
#******************************************************************************#
#****************#
#* イメージ設定 *#
#****************#
# OSイメージ
OS_IMG   = ../build/release/MochiOS.img
# ディスクイメージ
DISK_IMG = ../build/release/disk.img


#****************#
#* カーネル設定 *#
#****************#
# boot
BOOT_IPL  = kernel/build/release/booter-ipl.bin
BOOT_MAIN = kernel/build/release/booter-main.bin

# kernel
KERNEL = kernel/build/release/kernel

# ツール
MAKEIMG  = kernel/build/release/tool/makeimg
MAKEDISK = kernel/build/release/tool/makedisk

# Cフラグ
KERNEL_CFLAGS = -I${PWD}/lib/MLib/build/release/include

# LDフラグ
KERNEL_LDFLAGS = -L${PWD}/lib/MLib/build/release


#************************#
#* ドライバプロセス設定 *#
#************************#
# 実行ファイル
DRIVERS  = driver/mlog/build/release/mlog
DRIVERS += driver/drv-ns16550/build/release/drv-ns16550

# ドライバ用Cフラグ
DRIVER_CFLAGS  = -I${PWD}/server/mvfs/build/release/include
DRIVER_CFLAGS += -I${PWD}/driver/mlog/build/release/include
DRIVER_CFLAGS += -I${PWD}/../build/release/include
DRIVER_CFLAGS += -I${PWD}/lib/MLib/build/release/include
DRIVER_CFLAGS += -I${PWD}/kernel/build/release/include

# ドライバ用LDフラグ
DRIVER_LDFLAGS  = -L${PWD}/server/mvfs/build/release
DRIVER_LDFLAGS += -L${PWD}/driver/mlog/build/release
DRIVER_LDFLAGS += -L${PWD}/../build/release
DRIVER_LDFLAGS += -L${PWD}/lib/MLib/build/release
DRIVER_LDFLAGS += -L${PWD}/kernel/build/release


#**********************#
#* サーバプロセス設定 *#
#**********************#
# 実行ファイル
SERVERS  = server/mvfs/build/release/mvfs
SERVERS += server/mtty/build/release/mtty

# サーバ用Cフラグ
SERVER_CFLAGS  = -I${PWD}/server/mvfs/build/release/include
SERVER_CFLAGS += -I${PWD}/driver/mlog/build/release/include
SERVER_CFLAGS += -I${PWD}/../build/release/include
SERVER_CFLAGS += -I${PWD}/lib/MLib/build/release/include
SERVER_CFLAGS += -I${PWD}/kernel/build/release/include

# サーバ用LDフラグ
SERVER_LDFLAGS  = -L${PWD}/server/mvfs/build/release
SERVER_LDFLAGS += -L${PWD}/driver/mlog/build/release
SERVER_LDFLAGS += -L${PWD}/../build/release
SERVER_LDFLAGS += -L${PWD}/lib/MLib/build/release
SERVER_LDFLAGS += -L${PWD}/kernel/build/release


#**********************#
#* ユーザプロセス設定 *#
#**********************#
# 実行ファイル
USERS =

# Cフラグ
USER_CFLAGS =

# LDフラグ
USER_LDFLAGS =


#******************************************************************************#
#* phonyターゲット                                                            *#
#******************************************************************************#
# サブモジュールコンパイル
.PHONY: all
all: lib kernel mlog server driver Makefile

# イメージ作成
.PHONY: image
image: all $(DISK_IMG) Makefile

# 全生成ファイルの削除
.PHONY: clean
clean:
	$(MAKE) -C server/mvfs/build clean
	$(MAKE) -C driver/drv-ns16550/build clean
	$(MAKE) -C driver/mlog/build clean
	$(MAKE) -C kernel/build clean
	$(MAKE) -C lib/libc clean
	$(MAKE) -C lib/MLib/build clean
	-rm -rf $(OS_IMG) $(DISK_IMG)

# libコンパイル
.PHONY: lib
lib: Makefile
	$(MAKE) -C lib/MLib/build
	$(MAKE) -C lib/libc

# kernelコンパイル
.PHONY: kernel
kernel: Makefile
	env BASE_CFLAGS="$(KERNEL_CFLAGS)" BASE_LDFLAGS="$(KERNEL_LDFLAGS)" \
        $(MAKE) -C kernel/build

# mlogコンパイル
.PHONY: mlog
mlog: Makefile
	env BASE_CFLAGS="$(DRIVER_CFLAGS)" BASE_LDFLAGS="$(DRIVER_LDFLAGS)" \
        $(MAKE) -C driver/mlog/build

# driverコンパイル
.PHONY: driver
driver: Makefile
	env BASE_CFLAGS="$(DRIVER_CFLAGS)" BASE_LDFLAGS="$(DRIVER_LDFLAGS)" \
        $(MAKE) -C driver/drv-ns16550/build

# serverコンパイル
.PHONY: server
server: Makefile
	env BASE_CFLAGS="$(SERVER_CFLAGS)" BASE_LDFLAGS="$(SERVER_LDFLAGS)" \
        $(MAKE) -C server/mvfs/build
	env BASE_CFLAGS="$(SERVER_CFLAGS)" BASE_LDFLAGS="$(SERVER_LDFLAGS)" \
        $(MAKE) -C server/mtty/build


#******************************************************************************#
#* 生成規則                                                                   *#
#******************************************************************************#
# ディスクイメージ
$(DISK_IMG): $(MAKEDISK) $(BOOT_IPL) $(BOOT_MAIN) $(OS_IMG) Makefile
	$(MAKEDISK) -o $@ -i $(BOOT_IPL) -b $(BOOT_MAIN) -k $(OS_IMG)

# OSイメージ
$(OS_IMG): $(MAKEIMG) $(KERNEL) $(DRIVERS) $(SERVERS) $(USERS) Makefile
	$(MAKEIMG) -o $@ -K $(KERNEL)
ifdef DRIVERS
	$(MAKEIMG) -o $@ -D $(DRIVERS)
endif
ifdef SERVERS
	$(MAKEIMG) -o $@ -S $(SERVERS)
endif
ifdef USERS
	$(MAKEIMG) -o $@ -U $(USERS)
endif

#******************************************************************************#
