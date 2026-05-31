#include "fileio.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int calculate_global_freq(const char **files, int cnt, uint64_t freq[]) {
    memset(freq,0,MAX_BYTE*8);
    for (int i=0;i<cnt;i++) {
        FILE *fp = fopen(files[i],"rb");
        if (!fp) return 0;
        int c; while((c=fgetc(fp))!=EOF) freq[c]++;
        fclose(fp);
    }
    return 1;
}

int compress_files(const char **files, int cnt, const char *out) {
    uint64_t freq[MAX_BYTE] = {0};
    calculate_global_freq(files,cnt,freq);

    HuffmanNode *root = build_huffman_tree(freq);
    HuffmanCode codes[MAX_BYTE] = {0};
    generate_huffman_codes(root,codes,0,0);

    FILE *fp = fopen(out,"wb");
    fwrite(&cnt,4,1,fp);

    FileMeta meta[cnt];
    for (int i=0;i<cnt;i++) {
        strcpy(meta[i].filename,files[i]);
        FILE *f = fopen(files[i],"rb"); fseek(f,0,SEEK_END);
        meta[i].size = ftell(f); fclose(f);
        fwrite(&meta[i],sizeof(FileMeta),1,fp);
    }

    serialize_huffman_tree(root,fp);

    uint8_t buf=0, cnt_bit=0;
    for (int i=0;i<cnt;i++) {
        FILE *f = fopen(files[i],"rb");
        int c; while((c=fgetc(f))!=EOF) encode_byte(c,codes,fp,&buf,&cnt_bit);
        fclose(f);
    }
    if (cnt_bit) { buf <<= (8-cnt_bit); fputc(buf,fp); }

    fclose(fp);
    free_huffman_tree(root);
    return 1;
}

int decompress_file(const char *in) {
    FILE *fp = fopen(in,"rb");
    int cnt; fread(&cnt,4,1,fp);

    FileMeta meta[cnt];
    for (int i=0;i<cnt;i++) fread(&meta[i],sizeof(FileMeta),1,fp);

    HuffmanNode *root = deserialize_huffman_tree(fp);
    uint8_t buf=0, cnt_bit=0;

    for (int i=0;i<cnt;i++) {
        FILE *out = fopen(meta[i].filename,"wb");
        HuffmanNode *cur = root;
        uint64_t w=0; uint8_t b;
        while (w < meta[i].size) {
            if (decode_byte(&cur,fp,&buf,&cnt_bit,&b)) {
                fputc(b,out); w++; cur=root;
            } else break;
        }
        fclose(out);
    }

    fclose(fp);
    free_huffman_tree(root);
    return 1;
}
