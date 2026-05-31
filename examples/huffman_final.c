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
    uint32_t code;
    uint8_t len;
} HuffmanCode;

typedef struct {
    HuffmanNode **arr;
    int size;
} Heap;

Heap* create_heap(int cap) {
    Heap* h = malloc(sizeof(Heap));
    h->arr = malloc(cap * sizeof(HuffmanNode*));
    h->size = 0;
    return h;
}

void swap(HuffmanNode** a, HuffmanNode** b) {
    HuffmanNode* t = *a;
    *a = *b;
    *b = t;
}

void heapify(Heap* h, int i) {
    int small = i;
    int l = 2*i+1, r = 2*i+2;
    if (l < h->size && h->arr[l]->freq < h->arr[small]->freq) small = l;
    if (r < h->size && h->arr[r]->freq < h->arr[small]->freq) small = r;
    if (small != i) {
        swap(&h->arr[i], &h->arr[small]);
        heapify(h, small);
    }
}

void push(Heap* h, HuffmanNode* n) {
    h->arr[h->size++] = n;
    for (int i = h->size-1; i>0; i=(i-1)/2) {
        if (h->arr[(i-1)/2]->freq > h->arr[i]->freq)
            swap(&h->arr[i], &h->arr[(i-1)/2]);
        else break;
    }
}

HuffmanNode* pop(Heap* h) {
    HuffmanNode* n = h->arr[0];
    h->arr[0] = h->arr[--h->size];
    heapify(h,0);
    return n;
}

HuffmanNode* build_huffman_tree(uint64_t freq[]) {
    Heap* h = create_heap(256);
    for (int i=0;i<256;i++) {
        if (freq[i] > 0) {
            HuffmanNode* n = malloc(sizeof(HuffmanNode));
            n->byte = i;
            n->freq = freq[i];
            n->left = n->right = NULL;
            push(h,n);
        }
    }
    while (h->size > 1) {
        HuffmanNode* l = pop(h);
        HuffmanNode* r = pop(h);
        HuffmanNode* p = malloc(sizeof(HuffmanNode));
        p->byte = 0;
        p->freq = l->freq + r->freq;
        p->left = l;
        p->right = r;
        push(h,p);
    }
    HuffmanNode* root = pop(h);
    free(h->arr);
    free(h);
    return root;
}

void generate_codes(HuffmanNode* r, HuffmanCode c[], uint32_t code, uint8_t len) {
    if (!r) return;
    if (!r->left && !r->right) {
        c[r->byte].code = code;
        c[r->byte].len = len;
        return;
    }
    generate_codes(r->left, c, code<<1, len+1);
    generate_codes(r->right, c, (code<<1)|1, len+1);
}

void serialize_tree(HuffmanNode* r, FILE* fp) {
    if (!r) { fputc(0, fp); return; }
    fputc(1, fp);
    fputc(r->byte, fp);
    serialize_tree(r->left, fp);
    serialize_tree(r->right, fp);
}

void encode(uint8_t b, HuffmanCode c[], FILE* fp, uint8_t* buf, uint8_t* cnt) {
    uint32_t code = c[b].code;
    int len = c[b].len;
    for (int i=len-1;i>=0;i--) {
        *buf = (*buf << 1) | ((code >> i) & 1);
        (*cnt)++;
        if (*cnt == 8) {
            fputc(*buf, fp);
            *buf = 0;
            *cnt = 0;
        }
    }
}

int main() {
    printf("===== 霍夫曼压缩程序（最终版）=====\n\n");
    
    const char* input_file = "test.txt";
    const char* output_file = "test.huf";
    
    printf("正在检查输入文件：%s\n", input_file);
    
    // 1. 检查输入文件是否存在
    FILE* fp = fopen(input_file, "rb");
    if (!fp) {
        printf("\n错误：找不到文件 %s\n", input_file);
        printf("请在当前文件夹里新建一个名为 test.txt 的文件\n");
        printf("然后在里面随便写一些内容保存\n");
        goto end;
    }
    
    // 2. 统计字符频率
    uint64_t freq[256] = {0};
    int c;
    while ((c = fgetc(fp)) != EOF) {
        freq[c]++;
    }
    fclose(fp);
    printf("字符频率统计完成\n");
    
    // 3. 构建霍夫曼树
    HuffmanNode* root = build_huffman_tree(freq);
    printf("霍夫曼树构建完成\n");
    
    // 4. 生成霍夫曼编码表
    HuffmanCode codes[256] = {0};
    generate_codes(root, codes, 0, 0);
    printf("霍夫曼编码表生成完成\n");
    
    // 5. 写入压缩文件
    FILE* out = fopen(output_file, "wb");
    if (!out) {
        printf("\n错误：无法创建输出文件 %s\n", output_file);
        printf("可能是文件夹权限不足或者文件正在被占用\n");
        goto end;
    }
    
    // 写入原文件大小
    fp = fopen(input_file, "rb");
    fseek(fp, 0, SEEK_END);
    uint64_t file_size = ftell(fp);
    fwrite(&file_size, sizeof(uint64_t), 1, out);
    fclose(fp);
    
    // 序列化霍夫曼树
    serialize_tree(root, out);
    
    // 写入压缩数据
    uint8_t bit_buf = 0, bit_cnt = 0;
    fp = fopen(input_file, "rb");
    while ((c = fgetc(fp)) != EOF) {
        encode((uint8_t)c, codes, out, &bit_buf, &bit_cnt);
    }
    // 写入最后不足8位的部分
    if (bit_cnt > 0) {
        bit_buf <<= (8 - bit_cnt);
        fputc(bit_buf, out);
    }
    
    fclose(fp);
    fclose(out);
    
    printf("\n压缩成功！\n");
    printf("生成的压缩文件：%s\n", output_file);
    printf("原文件大小：%llu 字节\n", file_size);
    
end:
    printf("\n按任意键退出程序...\n");
    system("pause");
    return 0;
}
