#ifndef COMMON_H
#define COMMON_H

struct huffman_binary_tree_node {
    unsigned long long sum;
    char symbol;
    struct huffman_binary_tree_node *left, *right, *parent, *next, *previous;
};

void parse_arguments(int argc, char **argv, unsigned long long *file_count, char ***filenames, char **output);
void verify_files(unsigned long long file_count, char **filenames);
void huffman_binary_tree_append(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node);
void huffman_binary_tree_remove(struct huffman_binary_tree_node **head, struct huffman_binary_tree_node **tail, struct huffman_binary_tree_node *node);

#endif // COMMON_H
