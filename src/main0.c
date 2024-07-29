#define _GNU_SOURCE
#ifndef SRC_MAIN_SRC
#define SRC_MAIN_SRC

#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(unsigned long long int argc, char **argv)
{
	if(!argc)
		return 1;
	if(!argv)
		return 1;
	static struct
	{
		unsigned long long int string_count;
		char **strings;
		unsigned long long int *string_sizes;
		char *output;
		FILE *file;
	}buffers;
	for(unsigned long long int argument_index=1; argument_index<argc; argument_index++)
		if(!strcmp(argv[argument_index], "-o"))
			if((++argument_index)>=argc)
				return 1;
			else
				if(buffers.output)
					return 1;
				else
					buffers.output=argv[argument_index];
		else
		{
			if(!++buffers.string_count)
				return 1;
			if(!(buffers.strings=realloc(buffers.strings, buffers.string_count*sizeof(char*))))
				return 1;
			buffers.strings[buffers.string_count-1]=argv[argument_index];
		}
	if(!buffers.string_count)
		return 1;
	if(!buffers.strings)
		return 1;
	if(buffers.output)
		if(buffers.string_count>1)
			return 1;
	else
		buffers.output="out";
	for(unsigned long long int string_buffer_index=0; string_buffer_index<buffers.string_count; string_buffer_index++)
	{
		if(!(buffers.file=fopen(buffers.strings[string_buffer_index], "rb")))
			return 1;
		if(fclose(buffers.file))
			return 1;
	}
	if(!(buffers.string_sizes=malloc(buffers.string_count*sizeof(unsigned long long int))))
		return 1;
	for(unsigned long long int string_buffer_index=0; string_buffer_index<buffers.string_count; string_buffer_index++)
	{
		if(!(buffers.file=fopen(buffers.strings[string_buffer_index], "rb")))
			return 1;
		if(fseek(buffers.file, 0, SEEK_END))
			return 1;
		if((buffers.string_sizes[string_buffer_index]=ftell(buffers.file))==-1L)
			return 1;
		if(fseek(buffers.file, 0, SEEK_SET))
			return 1;
		char *bytes;
		if(!(bytes=malloc(buffers.string_sizes[string_buffer_index]*sizeof(char))))
			return 1;
		if(buffers.string_sizes[string_buffer_index])
			if(fread(bytes, sizeof(char), buffers.string_sizes[string_buffer_index], buffers.file)<buffers.string_sizes[string_buffer_index])
				return 1;
		if(fclose(buffers.file))
			return 1;
		buffers.strings[string_buffer_index]=bytes;
	}
	
	return 0;
}

#endif//SRC_MAIN_SRC