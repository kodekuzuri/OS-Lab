#include "memlab.h"

map<int,int> var_table;
block *mem ; 


int size_of_datatype(int var_type)
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

	cout << p << "\n" ; 
	cout << mem << "\n" ; 
	cout << mem->size << "\n" ; 
	return ;
}

Datatype createVar(int var_type)
{
	// Find free block
	int size = size_of_datatype(var_type); // size is in bits
	int local_index = var_table.size() ; 

	// word Id 
	int wordId = mem->getMemory(size);
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
