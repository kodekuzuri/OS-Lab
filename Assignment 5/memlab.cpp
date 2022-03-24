#include "memlab.h"

void* createMem(int memory)
{
	void *p = malloc(memory);
	if(p==NULL)
	{
		perror("malloc");
		exit(0);
	}
	return p;
}

void createVar(int var_size)
{
	// Find free block

	// Allocate space for variable
}