#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
