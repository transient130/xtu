#include "cmdline.h"
#include <stdio.h>
#include <string.h>

void usage(char *n) {
    printf("用法:\n");
    printf("  压缩: %s -c -o 输出.huf 文件1 文件2\n",n);
    printf("  解压: %s -x 压缩包.huf\n",n);
}

int parse_cmd(int argc, char **argv, CmdArgs *args) {
    memset(args,0,sizeof(CmdArgs));
    args->compress = -1;
    strcpy(args->out,"out.huf");

    for (int i=1;i<argc;i++) {
        if (!strcmp(argv[i],"-c")) args->compress=1;
        else if (!strcmp(argv[i],"-x")) args->compress=0;
        else if (!strcmp(argv[i],"-o") && i+1<argc) strcpy(args->out,argv[++i]);
        else strcpy(args->in[args->in_cnt++],argv[i]);
    }
    return 1;
}
