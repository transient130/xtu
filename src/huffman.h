#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include <stdio.h>

#define MAX_BYTE 256

typedef struct HuffmanNode {
    uint8_t byte;
    uint64_t freq;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

typedef struct {
    uint32_t code;
    uint8_t length;
} HuffmanCode;

HuffmanNode* build_huffman_tree(uint64_t freq[]);
void generate_huffman_codes(HuffmanNode *root, HuffmanCode codes[], uint32_t code, uint8_t len);
void serialize_huffman_tree(HuffmanNode *root, FILE *fp);
HuffmanNode* deserialize_huffman_tree(FILE *fp);
void encode_byte(uint8_t byte, HuffmanCode codes[], FILE *fp, uint8_t *bit_buf, uint8_t *bit_cnt);
int decode_byte(HuffmanNode **cur, FILE *fp, uint8_t *bit_buf, uint8_t *bit_cnt, uint8_t *out);
void free_huffman_tree(HuffmanNode *root);

#endif
