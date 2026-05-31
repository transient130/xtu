#include "cmdline.h"
#include "fileio.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    system("chcp 65001 > nul");
    CmdArgs args;
    parse_cmd(argc, argv, &args);

    printf("===== 霍夫曼压缩/解压程序 =====\n");

    const char *files[100];
    for (int i = 0; i < args.in_cnt; i++) {
        files[i] = args.in[i];
    }

    if (args.compress == 1) {
        printf("开始压缩...\n");
        compress_files(files, args.in_cnt, args.out);
        printf("压缩成功！生成文件：%s\n", args.out);
    } else if (args.compress == 0) {
        printf("开始解压...\n");
        decompress_file(args.in[0]);
        printf("解压成功！\n");
    } else {
        printf("参数错误！\n");
        printf("用法：\n");
        printf("  压缩：huffman.exe -c -o 输出.huf 输入.txt\n");
        printf("  解压：huffman.exe -x 压缩包.huf\n");
    }

    return 0;
}
