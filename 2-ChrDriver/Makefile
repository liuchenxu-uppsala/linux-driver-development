# ==============================================================================
# Linux 内核驱动 Makefile (ChrDriver 项目)
#
# KERNELDIR: 内核源码的 WSL 路径。
# obj-m: 定义要编译的模块目标文件。
# ==============================================================================

# KERNELDIR: Linux 内核源码的路径 (这是您的 WSL 映射路径)
KERNELDIR := /mnt/c/Users/Chenx/Documents/linux-source/linux-source/linux-source

# CURRENT_PATH: 当前驱动代码所在目录的 WSL 路径
CURRENT_PATH := $(shell pwd)

# -------------------- 模块定义 --------------------

# obj-m 变量定义了要编译的模块目标文件 (.o 文件列表)
# 您的项目包含 driver.c 和 main.c，所以目标是 driver.o 和 main.o
# 注意：列表中的文件名不要带 .c 后缀，编译系统会自动处理
obj-m := driver.o

# -------------------- 交叉编译配置 (可选，但推荐) --------------------

# 如果您在 CLion 的 Toolchain 中没有全局设置 CROSS_COMPILE，可以在这里指定
# 假设您的 ARM 交叉编译工具前缀是 arm-linux-gnueabi-
# 如果 CLion Toolchain 已配置，可以省略或注释掉下面这行
 CROSS_COMPILE := arm-linux-gnueabi-

# ARCH: 目标架构 (如果您的内核源码是 ARM 架构，请取消注释)
 ARCH := arm

# -------------------- 目标定义 --------------------

# 默认目标 (当直接执行 make 时)
# 这里调用内核源码树的 Makefile 来执行编译
all: kernel_modules

# 模块编译目标
kernel_modules:
	# -C $(KERNELDIR): 切换到内核源码目录
	# M=$(CURRENT_PATH): 告诉内核 Makefile 实际的模块源码在哪里
	#$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
	bear -- make -C $(KERNELDIR) M=$(CURRENT_PATH) modules

# 清理目标
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

# 避免 make 误认为 all, kernel_modules, clean 是文件
.PHONY: all kernel_modules clean