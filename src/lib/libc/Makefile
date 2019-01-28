#******************************************************************************#
#* src/lib/libc/Makefile                                                      *#
#*                                                                 2019/01/28 *#
#* Copyright (C) 2016-2019 Mochi.                                             *#
#******************************************************************************#
#******************************************************************************#
#* 設定                                                                       *#
#******************************************************************************#
# ライブラリ名
LIB = c

# ソースコード
SRCS = stdlib/malloc.c  \
       stdlib/free.c    \
       string/memcpy.c  \
       string/memset.c  \
       string/strcpy.c  \
       string/strncmp.c \
       string/strncpy.c \
       string/strlen.c

# ビルドディレクトリ
BUILD_DIR = ../../../build
# オブジェクトディレクトリ
OBJ_DIR   = $(BUILD_DIR)/obj/lib/libc

# Cフラグ
CFLAGS = -O                     \
         -Wall                  \
         -masm=intel            \
         -m32                   \
         -ffreestanding         \
         -Iinclude              \
         -I$(BUILD_DIR)/include


#******************************************************************************#
#* 定義                                                                       *#
#******************************************************************************#
# オブジェクトサブディレクトリ
OBJ_SUBDIRS = $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

# オブジェクトファイル
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

# 依存関係ファイル
DEPS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.d))


#******************************************************************************#
#* phonyターゲット                                                            *#
#******************************************************************************#
# コンパイル
.PHONY: all
all: $(OBJ_SUBDIRS) $(OBJ_DIR)/lib$(LIB).a $(BUILD_DIR)/lib$(LIB).a Makefile

# 全生成ファイルの削除
.PHONY: clean
clean:
	-rm -rf $(OBJ_DIR)
	-rm -rf $(BUILD_DIR)/lib$(LIB).a


#******************************************************************************#
#* 生成規則                                                                   *#
#******************************************************************************#
# 依存関係
-include $(DEPS)

# オブジェクトサブディレクトリ
$(OBJ_SUBDIRS):
	mkdir -p $@

# ライブラリ
$(BUILD_DIR)/lib$(LIB).a: $(OBJ_DIR)/lib$(LIB).a Makefile
	ln -sfr $< $@

$(OBJ_DIR)/lib$(LIB).a: $(OBJS) Makefile
	$(AR) rcs $@ $(OBJS)

# アセンブラファイルコンパイル
$(OBJ_DIR)/%.o: %.s Makefile
	$(AS) -o $@ $<

# Cファイルコンパイル
$(OBJ_DIR)/%.o: %.c Makefile
	$(CC) $(CFLAGS) -o $@ -c $< -MD -MP


#******************************************************************************#
