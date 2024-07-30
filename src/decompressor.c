#define _GNU_SOURCE
#ifndef SRC_MAIN_SRC
#define SRC_MAIN_SRC

#include "decompressor.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void load_compressed_data(char *filename, unsigned long long *string_size, unsigned long long *byte_count, char **compressed_data, struct huffman_binary_tree_node **tree);
struct huffman_binary_tree_node *read_huffman_tree(FILE *file);
void decode_data(struct huffman_binary_tree_node *tree, char *compressed_data, unsigned long long byte_count, unsigned long long string_size, char *output);
void binary_data_read(unsigned char *bit_index, unsigned long long *byte_index, char **bytes, unsigned char *bit);

void decompress_file(const char *filename, const char *output) {
    unsigned long long string_size;
    unsigned long long byte_count;
    char *compressed_data;
    struct huffman_binary_tree_node *tree;

    load_compressed_data((char *)filename, &string_size, &byte_count, &compressed_data, &tree);
    decode_data(tree, compressed_data, byte_count, string_size, (char *)output);
    free(compressed_data);
    while (tree) {
        struct huffman_binary_tree_node *node = tree;
        tree = tree->next;
        free(node);
    }
}


void load_compressed_data(char *filename, unsigned long long *string_size, unsigned long long *byte_count, char **compressed_data, struct huffman_binary_tree_node **tree) {
    FILE *file = fopen(filename, "rb");
    assert(file);
    fread(string_size, sizeof(unsigned long long), 1, file);
    *tree = read_huffman_tree(file);
    fread(byte_count, sizeof(unsigned long long), 1, file);
    *compressed_data = malloc(*byte_count);
    assert(*compressed_data);
    fread(*compressed_data, 1, *byte_count, file);
    fclose(file);
}

struct huffman_binary_tree_node *read_huffman_tree(FILE *file) {
    struct huffman_binary_tree_node *node = NULL;
    int c = fgetc(file);
    if (c == '1') {
        node = calloc(1, sizeof(struct huffman_binary_tree_node));
        node->symbol = fgetc(file);
    } else if (c == '0') {
        node = calloc(1, sizeof(struct huffman_binary_tree_node));
        node->left = read_huffman_tree(file);
        node->right = read_huffman_tree(file);
    }
    return node;
}

void decode_data(struct huffman_binary_tree_node *tree, char *compressed_data, unsigned long long byte_count, unsigned long long string_size, char *output) {
    char *output_filename = output ? strdup(output) : "output.txt";
    FILE *file = fopen(output_filename, "wb");
    assert(file);

    struct huffman_binary_tree_node *node = tree;
    unsigned long long byte_index = 0;
    unsigned char bit_index = 0;

    for (unsigned long long i = 0; i < string_size; i++) {
        while (node->left || node->right) {
            unsigned char bit;
            binary_data_read(&bit_index, &byte_index, &compressed_data, &bit);
            if (bit) node = node->right;
            else node = node->left;
        }
        fputc(node->symbol, file);
        node = tree;
    }

    fclose(file);
    if (!output) free(output_filename);
}

void binary_data_read(unsigned char *bit_index, unsigned long long *byte_index, char **bytes, unsigned char *bit) {
    *bit = ((*bytes)[*byte_index] >> (7 - (*bit_index)++)) & 1;
    if (*bit_index == 8) {
        (*bit_index) = 0;
        (*byte_index)++;
    }
}

#endif // SRC_MAIN_SRC
