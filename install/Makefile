#******************************************************************************#
#*                                                                            *#
#* install/Makefile                                                           *#
#*                                                                 2019/04/12 *#
#* Copyright (C) 2019 Mochi                                                   *#
#*                                                                            *#
#******************************************************************************#
#******************************************************************************#
#* 設定                                                                       *#
#******************************************************************************#
# boot
BOOT_IPL  = boot/booter-ipl.bin
BOOT_MAIN = boot/booter-main.bin

# kernel
KERNEL = kernel/kernel

# ドライバプロセス
DRIVERS  = driver/mlog
DRIVERS += driver/drv-ns16550
# サーバプロセス
SERVERS  = server/mvfs
# ユーザプロセス
USERS    =

# OSイメージ
OS_IMG   = MochiOS.img
# ディスクイメージ
DISK_IMG = disk.img

# ツール
MAKEIMG  = ../build/tool/makeimg
MAKEDISK = ../build/tool/makedisk


#******************************************************************************#
#* phonyターゲット                                                            *#
#******************************************************************************#
# イメージ作成
.PHONY: image
image: $(DISK_IMG) Makefile

# 全生成ファイルの削除
.PHONY: clean
clean:
	-rm -rf $(OS_IMG) $(DISK_IMG)


#******************************************************************************#
#* 生成規則                                                                   *#
#******************************************************************************#
# ディスクイメージ
$(DISK_IMG): $(BOOT_IPL) $(BOOT_MAIN) $(OS_IMG) Makefile
	$(MAKEDISK) -o $@ -i $(BOOT_IPL) -b $(BOOT_MAIN) -k $(OS_IMG)

# OSイメージ
$(OS_IMG): $(KERNEL) $(DRIVERS) $(SERVERS) $(USERS) Makefile
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
