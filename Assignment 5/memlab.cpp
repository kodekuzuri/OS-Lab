#include "memlab.h"

vector<int> var_table;

int size_of_datatype(int var_type)
{
	int datatype_sizes[4] = {32,8,24,1};

	return datatype_sizes[var_type];
}
void* createMem(int memory)
{
	void *p = malloc(memory*8); //malloc takes mem in bytes
	if(p==NULL)
	{
		perror("malloc");
		exit(0);
	}
	return p;
}

Datatype createVar(int var_type)
{
	// Find free block
	int size = size_of_datatype(var_type);

	// Allocate space for variable

	return Datatype(local_index,var_type);
}
