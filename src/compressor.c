#define _GNU_SOURCE
#ifndef SRC_MAIN_SRC
#define SRC_MAIN_SRC

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct buffers {
    unsigned long long int file_count;
    char **filenames;
    char **file_strings;
    unsigned long long int *string_sizes;
    char *output;
};

struct huffman_binary_tree_node {
    unsigned long long int sum;
    char symbol;
    struct huffman_binary_tree_node *left;
    struct huffman_binary_tree_node *right;
    struct huffman_binary_tree_node *parent;
    struct huffman_binary_tree_node *next;
    struct huffman_binary_tree_node *previous;
};

void parse_arguments(int argc, char **argv, struct buffers *buffers);
void verify_files(struct buffers *buffers);
void load_file_data(struct buffers *buffers);
void process_files(struct buffers *buffers);
void huffman_binary_tree_append(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node);
void huffman_binary_tree_remove(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node);
struct huffman_binary_tree_node *huffman_binary_tree_merge(struct huffman_binary_tree_node *node0, struct huffman_binary_tree_node *node1);
struct huffman_binary_tree_node *huffman_binary_tree_find_smallest(struct huffman_binary_tree_node *head, struct huffman_binary_tree_node *first_smallest);
void leafs_recursive_find(struct huffman_binary_tree_node *node, struct huffman_binary_tree_node ***leafs, unsigned short *count);

int main(int argc, char **argv) {
    assert(argc);
    assert(argv);

    struct buffers buffers = {0};

    parse_arguments(argc, argv, &buffers);
    verify_files(&buffers);
    load_file_data(&buffers);
    process_files(&buffers);

    return 0;
}

void parse_arguments(int argc, char **argv, struct buffers *buffers) {
    for (int argument_index = 1; argument_index < argc; argument_index++) {
        if (!strcmp(argv[argument_index], "-o")) {
            assert((++argument_index) < argc);
            assert(!buffers->output);
            buffers->output = argv[argument_index];
        } else {
            assert(++buffers->file_count);
            buffers->filenames = realloc(buffers->filenames, buffers->file_count * sizeof(char*));
            assert(buffers->filenames);
            buffers->filenames[buffers->file_count - 1] = argv[argument_index];
        }
    }
    if (buffers->output) {
        assert(buffers->file_count <= 1);
    }
    assert(buffers->file_count);
    assert(buffers->filenames);
}

void verify_files(struct buffers *buffers) {
    FILE *file;
    for (unsigned long long int string_buffer_index = 0; string_buffer_index < buffers->file_count; string_buffer_index++) {
        file = fopen(buffers->filenames[string_buffer_index], "rb");
        assert(file);
        fclose(file);
    }
}

void load_file_data(struct buffers *buffers) {
    buffers->string_sizes = malloc(buffers->file_count * sizeof(unsigned long long int));
    assert(buffers->string_sizes);
    buffers->file_strings = malloc(buffers->file_count * sizeof(char*));
    assert(buffers->file_strings);

    FILE *file;
    for (unsigned long long int string_buffer_index = 0; string_buffer_index < buffers->file_count; string_buffer_index++) {
        file = fopen(buffers->filenames[string_buffer_index], "rb");
        assert(file);
        fseek(file, 0, SEEK_END);
        buffers->string_sizes[string_buffer_index] = ftell(file);
        assert(buffers->string_sizes[string_buffer_index] != -1L);
        fseek(file, 0, SEEK_SET);
        char *bytes = malloc(buffers->string_sizes[string_buffer_index] * sizeof(char));
        assert(bytes);
        if (buffers->string_sizes[string_buffer_index]) {
            fread(bytes, sizeof(char), buffers->string_sizes[string_buffer_index], file);
        }
        fclose(file);
        buffers->file_strings[string_buffer_index] = bytes;
    }
}

void process_files(struct buffers *buffers) {
    struct huffman_binary_tree_node *huffman_binary_tree_head = NULL;
    struct huffman_binary_tree_node *huffman_binary_tree_tail = NULL;

    for (unsigned long long int string_buffer_index = 0; string_buffer_index < buffers->file_count; string_buffer_index++) {
        unsigned long long int byte_recurrences[256] = {0};

        for (unsigned long long int byte_index = 0; byte_index < buffers->string_sizes[string_buffer_index]; byte_index++) {
            byte_recurrences[(unsigned char)buffers->file_strings[string_buffer_index][byte_index]]++;
        }

        for (unsigned short byte_index = 0; byte_index < 256; byte_index++) {
            if (byte_recurrences[byte_index]) {
                struct huffman_binary_tree_node *node = calloc(1, sizeof(struct huffman_binary_tree_node));
                assert(node);
                node->sum = byte_recurrences[byte_index];
                node->symbol = byte_index;
                huffman_binary_tree_append(&huffman_binary_tree_head, &huffman_binary_tree_tail, node);
            }
        }

        while (huffman_binary_tree_head != huffman_binary_tree_tail) {
            struct huffman_binary_tree_node *smallest_node0 = huffman_binary_tree_find_smallest(huffman_binary_tree_head, NULL);
            struct huffman_binary_tree_node *smallest_node1 = huffman_binary_tree_find_smallest(huffman_binary_tree_head, smallest_node0);
            struct huffman_binary_tree_node *merged_node = huffman_binary_tree_merge(smallest_node0, smallest_node1);
            huffman_binary_tree_append(&huffman_binary_tree_head, &huffman_binary_tree_tail, merged_node);
            huffman_binary_tree_remove(&huffman_binary_tree_head, &huffman_binary_tree_tail, smallest_node0);
            huffman_binary_tree_remove(&huffman_binary_tree_head, &huffman_binary_tree_tail, smallest_node1);
        }

        assert(huffman_binary_tree_head && huffman_binary_tree_tail);

        struct huffman_binary_tree_node **leafs = NULL;
        unsigned short leaf_count = 0;
        leafs_recursive_find(huffman_binary_tree_head, &leafs, &leaf_count);

        struct {
            unsigned char bit_index;
            unsigned long long int byte_count;
            char *bytes;
        } binary_data = {7, 0, NULL};

        void binary_data_push(unsigned char bit) {
            if (!((++binary_data.bit_index) % 8)) {
                assert(++binary_data.byte_count);
                binary_data.bytes = realloc(binary_data.bytes, binary_data.byte_count * sizeof(char));
                assert(binary_data.bytes);
            }
            binary_data.bytes[binary_data.byte_count - 1] |= bit << binary_data.bit_index;
        }

        for (unsigned long long int string_byte_index = 0; string_byte_index < buffers->string_sizes[string_buffer_index]; string_byte_index++) {
            for (unsigned short leaf_index = 0; leaf_index < leaf_count; leaf_index++) {
                if (leafs[leaf_index]->symbol == buffers->file_strings[string_buffer_index][string_byte_index]) {
                    for (struct huffman_binary_tree_node *node = leafs[leaf_index], *node_aux = node->parent; node_aux; node = node->parent, node_aux = node_aux->parent) {
                        if (node == node_aux->left) {
                            binary_data_push(0);
                        } else if (node == node_aux->right) {
                            binary_data_push(1);
                        } else {
                            assert(0);
                        }
                    }
                    goto next_byte;
                }
            }
            assert(0);
        next_byte:;
        }

        if (!buffers->output) {
            buffers->output = strcat(buffers->filenames[string_buffer_index], ".a");
        }

        FILE *file = fopen(buffers->output, "wb");
        assert(file);
        fclose(file);
        file = fopen(buffers->output, "ab");
        assert(file);

        for (unsigned short byte_index = 0; byte_index < 256; byte_index++) {
            assert(fwrite(&byte_recurrences[byte_index], sizeof(unsigned long long int), 1, file) >= 1);
        }

        assert(binary_data.byte_count);
        assert(fwrite(binary_data.bytes, sizeof(char), binary_data.byte_count, file) >= binary_data.byte_count);
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
    if (*head == node) {
        *head = node->next;
    }
    if (*tail == node) {
        *tail = node->previous;
    }
    if (node->next) {
        node->next->previous = node->previous;
    }
    if (node->previous) {
        node->previous->next = node->next;
    }
}

struct huffman_binary_tree_node *huffman_binary_tree_merge(struct huffman_binary_tree_node *node0, struct huffman_binary_tree_node *node1) {
    struct huffman_binary_tree_node *self = calloc(1, sizeof(struct huffman_binary_tree_node));
    assert(self);
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
        if (!smallest) {
            if (first_smallest && (node == first_smallest)) {
                continue;
            }
            smallest = node;
        } else if (node->sum < smallest->sum) {
            if (first_smallest && (node == first_smallest)) {
                continue;
            }
            smallest = node;
        }
    }
    return smallest;
}

void leafs_recursive_find(struct huffman_binary_tree_node *node, struct huffman_binary_tree_node ***leafs, unsigned short *count) {
    assert(node);
    if (node->left) {
        leafs_recursive_find(node->left, leafs, count);
    }
    if (node->right) {
        leafs_recursive_find(node->right, leafs, count);
    }
    if (!(node->left || node->right)) {
        (*count)++;
        *leafs = realloc(*leafs, (*count) * sizeof(struct huffman_binary_tree_node*));
        assert(*leafs);
        (*leafs)[(*count) - 1] = node;
    }
}

#endif // SRC_MAIN_SRC
#define _GNU_SOURCE
#ifndef SRC_MAIN_SRC
#define SRC_MAIN_SRC

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct buffers {
    unsigned long long int file_count;
    char **filenames;
    char **file_strings;
    unsigned long long int *string_sizes;
    char *output;
};

struct huffman_binary_tree_node {
    unsigned long long int sum;
    char symbol;
    struct huffman_binary_tree_node *left;
    struct huffman_binary_tree_node *right;
    struct huffman_binary_tree_node *parent;
    struct huffman_binary_tree_node *next;
    struct huffman_binary_tree_node *previous;
};

void parse_arguments(int argc, char **argv, struct buffers *buffers);
void verify_files(struct buffers *buffers);
void load_file_data(struct buffers *buffers);
void process_files(struct buffers *buffers);
void huffman_binary_tree_append(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node);
void huffman_binary_tree_remove(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node);
struct huffman_binary_tree_node *huffman_binary_tree_merge(struct huffman_binary_tree_node *node0, struct huffman_binary_tree_node *node1);
struct huffman_binary_tree_node *huffman_binary_tree_find_smallest(struct huffman_binary_tree_node *head, struct huffman_binary_tree_node *first_smallest);
void leafs_recursive_find(struct huffman_binary_tree_node *node, struct huffman_binary_tree_node ***leafs, unsigned short *count);

int main(int argc, char **argv) {
    assert(argc);
    assert(argv);

    struct buffers buffers = {0};

    parse_arguments(argc, argv, &buffers);
    verify_files(&buffers);
    load_file_data(&buffers);
    process_files(&buffers);

    return 0;
}

void parse_arguments(int argc, char **argv, struct buffers *buffers) {
    for (int argument_index = 1; argument_index < argc; argument_index++) {
        if (!strcmp(argv[argument_index], "-o")) {
            assert((++argument_index) < argc);
            assert(!buffers->output);
            buffers->output = argv[argument_index];
        } else {
            assert(++buffers->file_count);
            buffers->filenames = realloc(buffers->filenames, buffers->file_count * sizeof(char*));
            assert(buffers->filenames);
            buffers->filenames[buffers->file_count - 1] = argv[argument_index];
        }
    }
    if (buffers->output) {
        assert(buffers->file_count <= 1);
    }
    assert(buffers->file_count);
    assert(buffers->filenames);
}

void verify_files(struct buffers *buffers) {
    FILE *file;
    for (unsigned long long int string_buffer_index = 0; string_buffer_index < buffers->file_count; string_buffer_index++) {
        file = fopen(buffers->filenames[string_buffer_index], "rb");
        assert(file);
        fclose(file);
    }
}

void load_file_data(struct buffers *buffers) {
    buffers->string_sizes = malloc(buffers->file_count * sizeof(unsigned long long int));
    assert(buffers->string_sizes);
    buffers->file_strings = malloc(buffers->file_count * sizeof(char*));
    assert(buffers->file_strings);

    FILE *file;
    for (unsigned long long int string_buffer_index = 0; string_buffer_index < buffers->file_count; string_buffer_index++) {
        file = fopen(buffers->filenames[string_buffer_index], "rb");
        assert(file);
        fseek(file, 0, SEEK_END);
        buffers->string_sizes[string_buffer_index] = ftell(file);
        assert(buffers->string_sizes[string_buffer_index] != -1L);
        fseek(file, 0, SEEK_SET);
        char *bytes = malloc(buffers->string_sizes[string_buffer_index] * sizeof(char));
        assert(bytes);
        if (buffers->string_sizes[string_buffer_index]) {
            fread(bytes, sizeof(char), buffers->string_sizes[string_buffer_index], file);
        }
        fclose(file);
        buffers->file_strings[string_buffer_index] = bytes;
    }
}

void process_files(struct buffers *buffers) {
    struct huffman_binary_tree_node *huffman_binary_tree_head = NULL;
    struct huffman_binary_tree_node *huffman_binary_tree_tail = NULL;

    for (unsigned long long int string_buffer_index = 0; string_buffer_index < buffers->file_count; string_buffer_index++) {
        unsigned long long int byte_recurrences[256] = {0};

        for (unsigned long long int byte_index = 0; byte_index < buffers->string_sizes[string_buffer_index]; byte_index++) {
            byte_recurrences[(unsigned char)buffers->file_strings[string_buffer_index][byte_index]]++;
        }

        for (unsigned short byte_index = 0; byte_index < 256; byte_index++) {
            if (byte_recurrences[byte_index]) {
                struct huffman_binary_tree_node *node = calloc(1, sizeof(struct huffman_binary_tree_node));
                assert(node);
                node->sum = byte_recurrences[byte_index];
                node->symbol = byte_index;
                huffman_binary_tree_append(&huffman_binary_tree_head, &huffman_binary_tree_tail, node);
            }
        }

        while (huffman_binary_tree_head != huffman_binary_tree_tail) {
            struct huffman_binary_tree_node *smallest_node0 = huffman_binary_tree_find_smallest(huffman_binary_tree_head, NULL);
            struct huffman_binary_tree_node *smallest_node1 = huffman_binary_tree_find_smallest(huffman_binary_tree_head, smallest_node0);
            struct huffman_binary_tree_node *merged_node = huffman_binary_tree_merge(smallest_node0, smallest_node1);
            huffman_binary_tree_append(&huffman_binary_tree_head, &huffman_binary_tree_tail, merged_node);
            huffman_binary_tree_remove(&huffman_binary_tree_head, &huffman_binary_tree_tail, smallest_node0);
            huffman_binary_tree_remove(&huffman_binary_tree_head, &huffman_binary_tree_tail, smallest_node1);
        }

        assert(huffman_binary_tree_head && huffman_binary_tree_tail);

        struct huffman_binary_tree_node **leafs = NULL;
        unsigned short leaf_count = 0;
        leafs_recursive_find(huffman_binary_tree_head, &leafs, &leaf_count);

        struct {
            unsigned char bit_index;
            unsigned long long int byte_count;
            char *bytes;
        } binary_data = {7, 0, NULL};

        void binary_data_push(unsigned char bit) {
            if (!((++binary_data.bit_index) % 8)) {
                assert(++binary_data.byte_count);
                binary_data.bytes = realloc(binary_data.bytes, binary_data.byte_count * sizeof(char));
                assert(binary_data.bytes);
            }
            binary_data.bytes[binary_data.byte_count - 1] |= bit << binary_data.bit_index;
        }

        for (unsigned long long int string_byte_index = 0; string_byte_index < buffers->string_sizes[string_buffer_index]; string_byte_index++) {
            for (unsigned short leaf_index = 0; leaf_index < leaf_count; leaf_index++) {
                if (leafs[leaf_index]->symbol == buffers->file_strings[string_buffer_index][string_byte_index]) {
                    for (struct huffman_binary_tree_node *node = leafs[leaf_index], *node_aux = node->parent; node_aux; node = node->parent, node_aux = node_aux->parent) {
                        if (node == node_aux->left) {
                            binary_data_push(0);
                        } else if (node == node_aux->right) {
                            binary_data_push(1);
                        } else {
                            assert(0);
                        }
                    }
                    goto next_byte;
                }
            }
            assert(0);
        next_byte:;
        }

        if (!buffers->output) {
            buffers->output = strcat(buffers->filenames[string_buffer_index], ".a");
        }

        FILE *file = fopen(buffers->output, "wb");
        assert(file);
        fclose(file);
        file = fopen(buffers->output, "ab");
        assert(file);

        for (unsigned short byte_index = 0; byte_index < 256; byte_index++) {
            assert(fwrite(&byte_recurrences[byte_index], sizeof(unsigned long long int), 1, file) >= 1);
        }

        assert(binary_data.byte_count);
        assert(fwrite(binary_data.bytes, sizeof(char), binary_data.byte_count, file) >= binary_data.byte_count);
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
    if (*head == node) {
        *head = node->next;
    }
    if (*tail == node) {
        *tail = node->previous;
    }
    if (node->next) {
        node->next->previous = node->previous;
    }
    if (node->previous) {
        node->previous->next = node->next;
    }
}

struct huffman_binary_tree_node *huffman_binary_tree_merge(struct huffman_binary_tree_node *node0, struct huffman_binary_tree_node *node1) {
    struct huffman_binary_tree_node *self = calloc(1, sizeof(struct huffman_binary_tree_node));
    assert(self);
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
        if (!smallest) {
            if (first_smallest && (node == first_smallest)) {
                continue;
            }
            smallest = node;
        } else if (node->sum < smallest->sum) {
            if (first_smallest && (node == first_smallest)) {
                continue;
            }
            smallest = node;
        }
    }
    return smallest;
}

void leafs_recursive_find(struct huffman_binary_tree_node *node, struct huffman_binary_tree_node ***leafs, unsigned short *count) {
    assert(node);
    if (node->left) {
        leafs_recursive_find(node->left, leafs, count);
    }
    if (node->right) {
        leafs_recursive_find(node->right, leafs, count);
    }
    if (!(node->left || node->right)) {
        (*count)++;
        *leafs = realloc(*leafs, (*count) * sizeof(struct huffman_binary_tree_node*));
        assert(*leafs);
        (*leafs)[(*count) - 1] = node;
    }
}

#endif // SRC_MAIN_SRC
