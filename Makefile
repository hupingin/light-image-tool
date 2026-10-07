# light-image-tool 构建脚本
#
# 两种产物(对应 README 的两种发布形态):
#   1) bin/lit          统一二进制, 适合直接当进程使用
#   2) lib/*.so|*.dll   按模块拆分的动态库, 适合大型项目按需集成
#
# 依赖: gcc / clang(Linux、macOS) 或 MinGW gcc(Windows)。
# 默认目标: 构建统一二进制。

CC      ?= gcc
CFLAGS  ?= -std=c99 -O2 -Wall -Isrc

# ---- 源文件 ----
CORE_SRCS := src/core/image.c
BMP_SRCS  := src/core/bmp.c
OPS_SRCS  := src/core/ops.c
MAIN_SRCS := src/main.c

# ---- 平台相关: 动态库后缀与编译标志 ----
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
  LIBEXT   := dll
  LIBFLAGS := -shared
else ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
  LIBEXT   := dll
  LIBFLAGS := -shared
else ifeq ($(UNAME_S),Darwin)
  LIBEXT   := dylib
  LIBFLAGS := -shared -fPIC
else
  LIBEXT   := so
  LIBFLAGS := -shared -fPIC
endif

.PHONY: all bin libs clean

all: bin

# ---- 统一二进制 ----
bin: bin/lit

bin/lit: $(CORE_SRCS) $(BMP_SRCS) $(OPS_SRCS) $(MAIN_SRCS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@

# ---- 按模块拆分的动态库 ----
libs: lib/liblitcore.$(LIBEXT) lib/liblitbmp.$(LIBEXT) lib/liblitops.$(LIBEXT)

lib/liblitcore.$(LIBEXT): $(CORE_SRCS)
	@mkdir -p lib
	$(CC) $(CFLAGS) $(LIBFLAGS) $^ -o $@

lib/liblitbmp.$(LIBEXT): $(BMP_SRCS) $(CORE_SRCS)
	@mkdir -p lib
	$(CC) $(CFLAGS) $(LIBFLAGS) $^ -o $@

lib/liblitops.$(LIBEXT): $(OPS_SRCS) $(CORE_SRCS)
	@mkdir -p lib
	$(CC) $(CFLAGS) $(LIBFLAGS) $^ -o $@

clean:
	rm -rf bin lib
