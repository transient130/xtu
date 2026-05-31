#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_BYTE 256

typedef struct HuffmanNode {
    uint8_t byte;
    uint64_t freq;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

typedef struct {
    HuffmanNode **arr;
    int size;
} Heap;

HuffmanNode* deserialize_tree(FILE* fp) {
    int f = fgetc(fp);
    if (f == 0) return NULL;
    HuffmanNode* n = malloc(sizeof(HuffmanNode));
    n->byte = fgetc(fp);
    n->left = deserialize_tree(fp);
    n->right = deserialize_tree(fp);
    return n;
}

int decode(HuffmanNode** cur, FILE* fp, uint8_t* buf, uint8_t* cnt, uint8_t* out) {
    while (1) {
        if (!(*cur)->left && !(*cur)->right) {
            *out = (*cur)->byte;
            return 1;
        }
        if (*cnt == 0) {
            int ch = fgetc(fp);
            if (ch == EOF) return 0;
            *buf = ch;
            *cnt = 8;
        }
        uint8_t bit = (*buf >> 7) & 1;
        *buf <<= 1;
        (*cnt)--;
        *cur = bit ? (*cur)->right : (*cur)->left;
    }
}

int main() {
    printf("===== 霍夫曼解压程序 =====\n\n");
    
    const char* input_file = "test.huf";
    const char* output_file = "test_decoded.txt";
    
    printf("正在读取压缩文件：%s\n", input_file);
    
    FILE* fp = fopen(input_file, "rb");
    if (!fp) {
        printf("\n错误：找不到压缩文件 %s\n", input_file);
        printf("请确保文件夹里有 test.huf 文件\n");
        goto end;
    }
    
    // 读取原文件大小
    uint64_t file_size;
    fread(&file_size, sizeof(uint64_t), 1, fp);
    printf("原文件大小：%llu 字节\n", file_size);
    
    // 反序列化霍夫曼树
    HuffmanNode* root = deserialize_tree(fp);
    printf("霍夫曼树重建完成\n");
    
    // 解压数据
    FILE* out = fopen(output_file, "wb");
    if (!out) {
        printf("\n错误：无法创建输出文件\n");
        goto end;
    }
    
    uint8_t bit_buf = 0, bit_cnt = 0;
    HuffmanNode* cur = root;
    uint64_t bytes_written = 0;
    uint8_t byte;
    
    while (bytes_written < file_size) {
        if (decode(&cur, fp, &bit_buf, &bit_cnt, &byte)) {
            fputc(byte, out);
            bytes_written++;
            cur = root;
        } else {
            break;
        }
    }
    
    fclose(fp);
    fclose(out);
    
    printf("\n解压成功！\n");
    printf("生成的文件：%s\n", output_file);
    printf("解压大小：%llu 字节\n", bytes_written);
    
end:
    printf("\n按任意键退出程序...\n");
    system("pause");
    return 0;
}
