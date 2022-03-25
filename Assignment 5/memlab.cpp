#include "memlab.h"

map<int,int> var_table;
block *mem ; 


int size_of_datatype(int var_type) // returns in bits
{
	int datatype_sizes[4] = {32,8,24,1};

	return datatype_sizes[var_type];
}

void createMem(int memory)
{
	void *p = malloc(memory); //malloc takes mem in bytes
	if(p==NULL)
	{
		perror("malloc");
		exit(0);
	}
 
	mem = (block *) p ;  
	mem->size = memory ;
	mem->next = NULL ;
	mem->free_flag = 1 ; 
	mem->data = NULL ; 
	mem->padding = 0 ; 

	return ;
}

int get_memory(int memory_size)
{
	block* start = mem;
	while((start!=NULL) && ((start->free_flag==0) || (start->size<memory_size)))
		start = start + 
	
	if(memory_size<(4*8))
	{
		//
	}
	else
	{
		block* curr_block = NULL;
		curr_block->size = memory_size/8;
		curr_block->free_flag = 0;
		curr_block->data = NULL;
		curr_block->padding = 

	}	

	
}

Datatype createVar(int var_type)
{
	// Find free block
	int size = size_of_datatype(var_type); // size is in bits
	int local_index = var_table.size() ; 

	// word Id 
	int wordId = mem->get_memory(size);
	var_table.push_back(wordId);

	
	// Allocate space for variable

	return Datatype(local_index,var_type);
}



int main(){
	int size ; 

	cout << "Enter Size : " << "\n" ; 
	cin >> size ; 


	createMem(size) ; 
}
