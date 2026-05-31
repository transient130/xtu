# 编译器
CC = gcc
# 编译参数
CFLAGS = -Wall -O2
# 输出文件名
TARGET = huffman.exe
# 源代码目录
SRC_DIR = src
# 所有源文件
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/huffman.c $(SRC_DIR)/fileio.c $(SRC_DIR)/cmdline.c

# 默认目标：编译
all:
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS)

# 清理编译生成的文件
clean:
	del $(TARGET) *.o *.huf test_decoded.txt