#ifndef CMDLINE_H
#define CMDLINE_H

typedef struct {
    int compress;
    char out[256];
    char in[100][256];
    int in_cnt;
} CmdArgs;

int parse_cmd(int argc, char **argv, CmdArgs *args);
void usage(char *n);

#endif
