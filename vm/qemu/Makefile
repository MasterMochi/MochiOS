#******************************************************************************#
#*                                                                            *#
#* vm/qemu/Makefile                                                           *#
#*                                                                 2022/12/17 *#
#* Copyright (C) 2019-2022 Mochi.                                             *#
#*                                                                            *#
#******************************************************************************#
#******************************************************************************#
#* 設定                                                                       *#
#******************************************************************************#
# イメージ
IMG = ../../build/release/disk.img

# 共通オプション
QEMUOPT  = -drive file=$(IMG),format=raw
QEMUOPT += -monitor stdio
QEMUOPT += -serial telnet::4444,server,nowait
QEMUOPT += -serial telnet::4445,server,nowait
QEMUOPT += -no-reboot -d cpu_reset -no-shutdown


#******************************************************************************#
#* phonyターゲット                                                            *#
#******************************************************************************#
# qemu実行(オプション無し)
.PHONY: run
run:
	qemu-system-i386 $(QEMUOPT)

# qemu実行(curses)
.PHONY: run-curses
run-curses:
	qemu-system-i386 $(QEMUOPT) -curses

# qemu実行(vnc)
.PHONY: run-vnc
run-vnc:
	qemu-system-i386 $(QEMUOPT) -vnc :0

# qemu実行(gdb+vnc)
.PHONY: gdb-vnc
gdb-vnc:
	qemu-system-i386 $(QEMUOPT) -s -S -vnc :0


#******************************************************************************#
