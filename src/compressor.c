#define _GNU_SOURCE
#ifndef SRC_MAIN_SRC
#define SRC_MAIN_SRC

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct huffman_binary_tree_node {
    unsigned long long sum;
    char symbol;
    struct huffman_binary_tree_node *left, *right, *parent, *next, *previous;
};

void parse_arguments(int argc, char **argv, unsigned long long *file_count, char ***filenames, char **output);
void verify_files(unsigned long long file_count, char **filenames);
void load_file_data(unsigned long long file_count, char **filenames, unsigned long long **string_sizes, char ***file_strings);
void process_file(char *filename, unsigned long long string_size, char *file_string, char *output);
void huffman_binary_tree_append(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node);
void huffman_binary_tree_remove(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node);
struct huffman_binary_tree_node *huffman_binary_tree_merge(struct huffman_binary_tree_node *node0, struct huffman_binary_tree_node *node1);
struct huffman_binary_tree_node *huffman_binary_tree_find_smallest(struct huffman_binary_tree_node *head, struct huffman_binary_tree_node *first_smallest);
void leafs_recursive_find(struct huffman_binary_tree_node *node, struct huffman_binary_tree_node ***leafs, unsigned short *count);
void binary_data_push(unsigned char bit, unsigned char *bit_index, unsigned long long *byte_count, char **bytes);
void write_huffman_tree(struct huffman_binary_tree_node *node, FILE *file);
void generate_huffman_codes(struct huffman_binary_tree_node *node, char **codes, char *buffer, int depth);

int main(int argc, char **argv) {
    assert(argc > 1 && argv);
    unsigned long long file_count = 0;
    char **filenames = NULL, *output = NULL;
    parse_arguments(argc, argv, &file_count, &filenames, &output);
    verify_files(file_count, filenames);
    unsigned long long *string_sizes = NULL;
    char **file_strings = NULL;
    load_file_data(file_count, filenames, &string_sizes, &file_strings);
    for (unsigned long long i = 0; i < file_count; i++) {
        process_file(filenames[i], string_sizes[i], file_strings[i], output);
    }
    for (unsigned long long i = 0; i < file_count; i++) free(file_strings[i]);
    free(file_strings);
    free(string_sizes);
    if (output) free(output);
    return 0;
}

void parse_arguments(int argc, char **argv, unsigned long long *file_count, char ***filenames, char **output) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-o")) {
            assert((++i) < argc && !(*output));
            *output = strdup(argv[i]);
            assert(*output);
        } else {
            (*file_count)++;
            *filenames = realloc(*filenames, *file_count * sizeof(char*));
            assert(*filenames);
            (*filenames)[*file_count - 1] = argv[i];
        }
    }
    if (*output) assert(*file_count <= 1);
    assert(*file_count && *filenames);
}

void verify_files(unsigned long long file_count, char **filenames) {
    for (unsigned long long i = 0; i < file_count; i++) {
        FILE *file = fopen(filenames[i], "rb");
        assert(file);
        fclose(file);
    }
}

void load_file_data(unsigned long long file_count, char **filenames, unsigned long long **string_sizes, char ***file_strings) {
    *string_sizes = malloc(file_count * sizeof(unsigned long long));
    *file_strings = malloc(file_count * sizeof(char*));
    assert(*string_sizes && *file_strings);
    for (unsigned long long i = 0; i < file_count; i++) {
        FILE *file = fopen(filenames[i], "rb");
        assert(file);
        fseek(file, 0, SEEK_END);
        (*string_sizes)[i] = ftell(file);
        assert((*string_sizes)[i] != -1L);
        fseek(file, 0, SEEK_SET);
        (*file_strings)[i] = malloc((*string_sizes)[i]);
        assert((*file_strings)[i]);
        fread((*file_strings)[i], 1, (*string_sizes)[i], file);
        fclose(file);
    }
}

void process_file(char *filename, unsigned long long string_size, char *file_string, char *output) {
    struct huffman_binary_tree_node *head = NULL, *tail = NULL;
    unsigned long long byte_recurrences[256] = {0};
    for (unsigned long long i = 0; i < string_size; i++) byte_recurrences[(unsigned char)file_string[i]]++;
    for (unsigned short i = 0; i < 256; i++) {
        if (byte_recurrences[i]) {
            struct huffman_binary_tree_node *node = calloc(1, sizeof(struct huffman_binary_tree_node));
            node->sum = byte_recurrences[i];
            node->symbol = i;
            huffman_binary_tree_append(&head, &tail, node);
        }
    }
    while (head != tail) {
        struct huffman_binary_tree_node *n0 = huffman_binary_tree_find_smallest(head, NULL);
        struct huffman_binary_tree_node *n1 = huffman_binary_tree_find_smallest(head, n0);
        struct huffman_binary_tree_node *merged = huffman_binary_tree_merge(n0, n1);
        huffman_binary_tree_append(&head, &tail, merged);
        huffman_binary_tree_remove(&head, &tail, n0);
        huffman_binary_tree_remove(&head, &tail, n1);
    }
    
    char *codes[256] = {0};
    char buffer[256];
    generate_huffman_codes(head, codes, buffer, 0);
    
    struct { unsigned char bit_index; unsigned long long byte_count; char *bytes; } binary_data = {7, 0, NULL};
    for (unsigned long long i = 0; i < string_size; i++) {
        char *code = codes[(unsigned char)file_string[i]];
        for (char *p = code; *p; p++) {
            binary_data_push(*p == '1', &binary_data.bit_index, &binary_data.byte_count, &binary_data.bytes);
        }
    }
    
    char *output_filename = output ? strdup(output) : malloc(strlen(filename) + 3);
    if (!output) snprintf(output_filename, strlen(filename) + 3, "%s.a", filename);
    FILE *file = fopen(output_filename, "wb");
    assert(file);
    
    fwrite(&string_size, sizeof(unsigned long long), 1, file);
    write_huffman_tree(head, file);
    fwrite(&binary_data.byte_count, sizeof(unsigned long long), 1, file);
    fwrite(binary_data.bytes, 1, binary_data.byte_count, file);
    fclose(file);
    
    for (unsigned short i = 0; i < 256; i++) {
        if (codes[i]) free(codes[i]);
    }
    free(binary_data.bytes);
    free(output_filename);
    while (head) {
        struct huffman_binary_tree_node *node = head;
        head = head->next;
        free(node);
    }
}

void huffman_binary_tree_append(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node) {
    if (*tail) {
        (*tail)->next = node;
        node->previous = *tail;
    } else {
        *head = node;
    }
    *tail = node;
}

void huffman_binary_tree_remove(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node) {
    if (*head == node) *head = node->next;
    if (*tail == node) *tail = node->previous;
    if (node->next) node->next->previous = node->previous;
    if (node->previous) node->previous->next = node->next;
}

struct huffman_binary_tree_node *huffman_binary_tree_merge(struct huffman_binary_tree_node *node0, struct huffman_binary_tree_node *node1) {
    struct huffman_binary_tree_node *self = calloc(1, sizeof(struct huffman_binary_tree_node));
    self->sum = node0->sum + node1->sum;
    self->left = node0;
    self->right = node1;
    node0->parent = self;
    node1->parent = self;
    return self;
}

struct huffman_binary_tree_node *huffman_binary_tree_find_smallest(struct huffman_binary_tree_node *head, struct huffman_binary_tree_node *first_smallest) {
    struct huffman_binary_tree_node *smallest = NULL;
    for (struct huffman_binary_tree_node *node = head; node; node = node->next) {
        if (!smallest && node != first_smallest) smallest = node;
        else if (smallest && node->sum < smallest->sum && node != first_smallest) smallest = node;
    }
    return smallest;
}

void leafs_recursive_find(struct huffman_binary_tree_node *node, struct huffman_binary_tree_node ***leafs, unsigned short *count) {
    if (!node) return;
    if (!node->left && !node->right) {
        (*count)++;
        *leafs = realloc(*leafs, (*count) * sizeof(struct huffman_binary_tree_node*));
        (*leafs)[(*count) - 1] = node;
    } else {
        leafs_recursive_find(node->left, leafs, count);
        leafs_recursive_find(node->right, leafs, count);
    }
}

void binary_data_push(unsigned char bit, unsigned char *bit_index, unsigned long long *byte_count, char **bytes) {
    if (!((++(*bit_index)) % 8)) {
        (*byte_count)++;
        *bytes = realloc(*bytes, (*byte_count) * sizeof(char));
        (*bytes)[(*byte_count) - 1] = 0;
    }
    (*bytes)[(*byte_count) - 1] |= bit << (7 - (*bit_index % 8));
}

void generate_huffman_codes(struct huffman_binary_tree_node *node, char **codes, char *buffer, int depth) {
    if (!node) return;
    if (!node->left && !node->right) {
        buffer[depth] = '\0';
        codes[(unsigned char)node->symbol] = strdup(buffer);
    } else {
        buffer[depth] = '0';
        generate_huffman_codes(node->left, codes, buffer, depth + 1);
        buffer[depth] = '1';
        generate_huffman_codes(node->right, codes, buffer, depth + 1);
    }
}

void write_huffman_tree(struct huffman_binary_tree_node *node, FILE *file) {
    if (!node) return;
    if (!node->left && !node->right) {
        fputc('1', file);
        fputc(node->symbol, file);
    } else {
        fputc('0', file);
        write_huffman_tree(node->left, file);
        write_huffman_tree(node->right, file);
    }
}

#endif // SRC_MAIN_SRC
