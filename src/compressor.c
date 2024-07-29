#define _GNU_SOURCE
#ifndef SRC_MAIN_SRC
#define SRC_MAIN_SRC

#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

int main(unsigned long long int argc, char **argv)
{
	assert(argc);
	assert(argv);

static struct
{
	unsigned long long int file_count;
	char **filenames;
	char **file_strings;
	unsigned long long int *string_sizes;
	char *output;
}buffers;

	{
		for(unsigned long long int argument_index=1; argument_index<argc; argument_index++)
			if(!strcmp(argv[argument_index], "-o"))
			{
				assert((++argument_index)<argc);
				assert(!buffers.output);
				buffers.output=argv[argument_index];
			}			
			else
			{
				assert(++buffers.file_count);
				assert(buffers.filenames=realloc(buffers.filenames, buffers.file_count*sizeof(char*)));
				buffers.filenames[buffers.file_count-1]=argv[argument_index];
			}
		if(buffers.output)
			assert(buffers.file_count<=1);
		assert(buffers.file_count);
		assert(buffers.filenames);
	}

	{
		FILE *file;
		for(unsigned long long int string_buffer_index=0; string_buffer_index<buffers.file_count; string_buffer_index++)
		{
			assert(file=fopen(buffers.filenames[string_buffer_index], "rb"));
			assert(!fclose(file));
		}
	}

	{
		assert(buffers.string_sizes=malloc(buffers.file_count*sizeof(unsigned long long int)));
		assert(buffers.file_strings=malloc(buffers.file_count*sizeof(char*)));
	}

	{
		FILE *file;
		for(unsigned long long int string_buffer_index=0; string_buffer_index<buffers.file_count; string_buffer_index++)
		{
			assert(file=fopen(buffers.filenames[string_buffer_index], "rb"));
			assert(!fseek(file, 0, SEEK_END));
			assert((buffers.string_sizes[string_buffer_index]=ftell(file))!=-1L);
			assert(!fseek(file, 0, SEEK_SET));
			char *bytes;
			assert(bytes=malloc(buffers.string_sizes[string_buffer_index]*sizeof(char)));
			if(buffers.string_sizes[string_buffer_index])
				assert(fread(bytes, sizeof(char), buffers.string_sizes[string_buffer_index], file)>=buffers.string_sizes[string_buffer_index]);
			assert(!fclose(file));
			buffers.file_strings[string_buffer_index]=bytes;
		}
	}
	
	{
		for(unsigned long long int string_buffer_index=0; string_buffer_index<buffers.file_count; string_buffer_index++)
		{

struct huffman_binary_tree_node
{
	unsigned long long int sum;
	char symbol;
	struct huffman_binary_tree_node *left;
	struct huffman_binary_tree_node *right;
	struct huffman_binary_tree_node *parent;
	struct huffman_binary_tree_node *next;
	struct huffman_binary_tree_node *previous;
}*huffman_binary_tree_head=NULL, *huffman_binary_tree_tail=NULL;
auto void huffman_binary_tree_append(struct huffman_binary_tree_node*);
auto void huffman_binary_tree_remove(struct huffman_binary_tree_node*);
auto struct huffman_binary_tree_node *huffman_binary_tree_merge(struct huffman_binary_tree_node*, struct huffman_binary_tree_node*);
auto struct huffman_binary_tree_node *huffman_binary_tree_find_smallest(struct huffman_binary_tree_node*);

			unsigned long long int byte_recurrences[256]={0};
			{
				{
					for(unsigned long long int byte_index=0; byte_index<buffers.string_sizes[string_buffer_index]; byte_index++)
						byte_recurrences[buffers.file_strings[string_buffer_index][byte_index]]++;
					for(unsigned short byte_index=0; byte_index<256; byte_index++)
						if(byte_recurrences[byte_index])
						{
							struct huffman_binary_tree_node *node;
							assert(node=calloc(1, sizeof(struct huffman_binary_tree_node)));
							node->sum=byte_recurrences[byte_index];
							node->symbol=byte_index;
							huffman_binary_tree_append(node);
						}
					while(huffman_binary_tree_head!=huffman_binary_tree_tail)
					{
						struct huffman_binary_tree_node *smallest_node0=huffman_binary_tree_find_smallest(NULL);
						struct huffman_binary_tree_node *smallest_node1=huffman_binary_tree_find_smallest(smallest_node0);
						huffman_binary_tree_append(huffman_binary_tree_merge(smallest_node0, smallest_node1));
						huffman_binary_tree_remove(smallest_node0);
						huffman_binary_tree_remove(smallest_node1);
					}
					assert(huffman_binary_tree_head&&huffman_binary_tree_tail);
				}
struct
{
	unsigned short count;
	struct huffman_binary_tree_node **leafs;
}leafs={0};
void leafs_recursive_find(struct huffman_binary_tree_node *node)
{
	assert(node);
	if(node->left)
		leafs_recursive_find(node->left);
	if(node->right)
		leafs_recursive_find(node->right);
	if(!(node->left||node->right))
	{
		assert(++leafs.count);
		assert(leafs.leafs=realloc(leafs.leafs, leafs.count*sizeof(struct huffman_binary_tree_node*)));
		leafs.leafs[leafs.count-1]=node;
	}
}
				{
					leafs_recursive_find(huffman_binary_tree_head);
				}
struct
{
	unsigned char bit_index;
	unsigned long long int byte_count;
	char *bytes;
}binary_data={7, 0, NULL};
void binary_data_push(unsigned char bit)
{
	if(!((++binary_data.bit_index)%8))
	{
		assert(++binary_data.byte_count);
		assert(binary_data.bytes=realloc(binary_data.bytes, binary_data.byte_count*sizeof(char)));
	}
	binary_data.bytes[binary_data.byte_count-1]|=bit<<binary_data.bit_index;
}
unsigned char binary_data_pop(void)
{
	unsigned char out=(binary_data.bytes[binary_data.byte_count-1]>>binary_data.bit_index)&1;
	if(((--binary_data.bit_index)%8)==7)
		assert(--binary_data.byte_count);
	return out;
}
			{
				for(unsigned long long int string_byte_index=0; string_byte_index<buffers.string_sizes[string_buffer_index]; string_byte_index++)
					{
						for(unsigned short leaf_index=0; leaf_index<leafs.count; leaf_index++)
							if(leafs.leafs[leaf_index]->symbol==buffers.file_strings[string_buffer_index][string_byte_index])
							{
								for(struct huffman_binary_tree_node *node=leafs.leafs[leaf_index], *node_aux=node->parent; node_aux; ({node=node->parent; node_aux=node_aux->parent;}))
									if(node==node_aux->left)
										binary_data_push(0);
									else if(node==node_aux->right)
										binary_data_push(1);
									else
										assert(NULL);
								goto next_byte;
							}
						assert(NULL);
					next_byte:
					}
			}

			{
				if(!buffers.output)
					buffers.output=strcat(buffers.output, ".a");
				FILE *file;
				assert(file=fopen(buffers.output, "wb"));
				assert(!fclose(file));
				assert(file=fopen(buffers.output, "ab"));
				for(unsigned short byte_index=0; byte_index<256; byte_index++)
					assert(fwrite(&byte_recurrences[byte_index], sizeof(unsigned long long int), 1, file)>=1);
				assert(binary_data.byte_count);
				assert(fwrite(binary_data.bytes, sizeof(char), binary_data.byte_count, file)>=binary_data.byte_count);
				assert(!fclose(file));
			}

			}

void huffman_binary_tree_append(struct huffman_binary_tree_node *node)
{
	if(huffman_binary_tree_tail)
	{
		huffman_binary_tree_tail->next=node;
		node->previous=huffman_binary_tree_tail;
	}
	else
		huffman_binary_tree_head=node;
	huffman_binary_tree_tail=node;
}
void huffman_binary_tree_remove(struct huffman_binary_tree_node *node)
{
	if(huffman_binary_tree_head==node)
		huffman_binary_tree_head=node->next;
	if(huffman_binary_tree_tail==node)
		huffman_binary_tree_tail=node->previous;
	if(node->next)
		node->next->previous=node->previous;
	if(node->previous)
		node->previous->next=node->next;
}
struct huffman_binary_tree_node *huffman_binary_tree_merge(struct huffman_binary_tree_node *node0, struct huffman_binary_tree_node *node1)
{
	struct huffman_binary_tree_node *self;
	assert(self=calloc(1, sizeof(struct huffman_binary_tree_node)));
	self->sum=node0->sum+node1->sum;
	self->left=node0;
	self->right=node1;
	node0->parent=self;
	node1->parent=self;
	return self;
}
struct huffman_binary_tree_node *huffman_binary_tree_find_smallest(struct huffman_binary_tree_node *first_smallest)
{
	struct huffman_binary_tree_node *smallest=NULL;
	for(struct huffman_binary_tree_node *node=huffman_binary_tree_head; node; node=node->next)
	{
		if(!smallest)
		{
			if(first_smallest&&(node==first_smallest))
				continue;
			smallest=node;
			continue;
		}
		if(node->sum<smallest->sum)
		{
			if(first_smallest&&(node==first_smallest))
				continue;
			smallest=node;
		}
	}
	return smallest;
}

		}
	}

	{
		return 0;
	}
}

#endif//SRC_MAIN_SRC