#ifndef FILEIO_H
#define FILEIO_H

#include "huffman.h"

#define MAX_FILENAME 256

typedef struct {
    char filename[MAX_FILENAME];
    uint64_t size;
} FileMeta;

int calculate_global_freq(const char **files, int cnt, uint64_t freq[]);
int compress_files(const char **files, int cnt, const char *out);
int decompress_file(const char *in);

#endif
