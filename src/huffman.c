#include "huffman.h"
#include <stdlib.h>

typedef struct {
    HuffmanNode **nodes;
    int size;
} MinHeap;

static MinHeap* create_heap(int cap) {
    MinHeap *h = malloc(sizeof(MinHeap));
    h->nodes = malloc(cap * sizeof(HuffmanNode*));
    h->size = 0;
    return h;
}

static void swap(HuffmanNode **a, HuffmanNode **b) {
    HuffmanNode *t = *a; *a = *b; *b = t;
}

static void heapify(MinHeap *h, int i) {
    int small = i;
    int l = 2*i+1, r = 2*i+2;
    if (l < h->size && h->nodes[l]->freq < h->nodes[small]->freq) small = l;
    if (r < h->size && h->nodes[r]->freq < h->nodes[small]->freq) small = r;
    if (small != i) {
        swap(&h->nodes[i], &h->nodes[small]);
        heapify(h, small);
    }
}

static void push(MinHeap *h, HuffmanNode *n) {
    h->nodes[h->size++] = n;
    for (int i = h->size-1; i>0 && h->nodes[(i-1)/2]->freq > h->nodes[i]->freq; i=(i-1)/2)
        swap(&h->nodes[i], &h->nodes[(i-1)/2]);
}

static HuffmanNode* pop(MinHeap *h) {
    HuffmanNode *n = h->nodes[0];
    h->nodes[0] = h->nodes[--h->size];
    heapify(h,0);
    return n;
}

HuffmanNode* build_huffman_tree(uint64_t freq[]) {
    MinHeap *h = create_heap(MAX_BYTE);
    for (int i=0;i<MAX_BYTE;i++) if (freq[i]) {
        HuffmanNode *n = malloc(sizeof(HuffmanNode));
        n->byte = i;
        n->freq = freq[i];
        n->left = n->right = NULL;
        push(h, n);
    }
    while (h->size>1) {
        HuffmanNode *l=pop(h),*r=pop(h);
        HuffmanNode *p = malloc(sizeof(HuffmanNode));
        p->byte=0; p->freq=l->freq+r->freq; p->left=l; p->right=r;
        push(h,p);
    }
    HuffmanNode *root = pop(h);
    free(h->nodes); free(h);
    return root;
}

void generate_huffman_codes(HuffmanNode *r, HuffmanCode c[], uint32_t code, uint8_t len) {
    if (!r) return;
    if (!r->left && !r->right) { c[r->byte].code=code; c[r->byte].length=len; return; }
    generate_huffman_codes(r->left,c,code<<1,len+1);
    generate_huffman_codes(r->right,c,(code<<1)|1,len+1);
}

void serialize_huffman_tree(HuffmanNode *r, FILE *fp) {
    if (!r) { fputc(0,fp); return; }
    fputc(1,fp); fputc(r->byte,fp);
    serialize_huffman_tree(r->left,fp);
    serialize_huffman_tree(r->right,fp);
}

HuffmanNode* deserialize_huffman_tree(FILE *fp) {
    int f = fgetc(fp);
    if (!f) return NULL;
    HuffmanNode *n = malloc(sizeof(HuffmanNode));
    n->byte = fgetc(fp);
    n->left = deserialize_huffman_tree(fp);
    n->right = deserialize_huffman_tree(fp);
    return n;
}

void encode_byte(uint8_t b, HuffmanCode c[], FILE *fp, uint8_t *buf, uint8_t *cnt) {
    uint32_t code = c[b].code;
    int len = c[b].length;
    for (int i=len-1;i>=0;i--) {
        *buf = (*buf << 1) | ((code >> i) & 1);
        (*cnt)++;
        if (*cnt == 8) { fputc(*buf,fp); *buf=0; *cnt=0; }
    }
}

int decode_byte(HuffmanNode **cur, FILE *fp, uint8_t *buf, uint8_t *cnt, uint8_t *out) {
    while (1) {
        if (!(*cur)->left && !(*cur)->right) {
            *out = (*cur)->byte;
            return 1;
        }
        if (*cnt == 0) {
            int ch = fgetc(fp);
            if (ch == EOF) return 0;
            *buf = ch; *cnt =8;
        }
        uint8_t bit = (*buf >>7) &1;
        *buf <<=1; (*cnt)--;
        *cur = bit ? (*cur)->right : (*cur)->left;
    }
}

void free_huffman_tree(HuffmanNode *r) {
    if (!r) return;
    free_huffman_tree(r->left);
    free_huffman_tree(r->right);
    free(r);
}
