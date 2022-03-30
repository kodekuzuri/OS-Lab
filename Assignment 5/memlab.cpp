// Name: Aryan Mehta
// Roll No.: 19CS30006

// Name: Pranav Rajput
// Roll no.: 19CS30036

// Assignment: Assignment 5
// Date: 22.03.2022
// Group: 5


// // TODO:
// 	Check if everything properly stored in word format or byte format everywhere
// 	medium int
// 	create array and assign array
// define max_stack_size and max_var_size as total_mem/12 in the end
// Dont use compaction for every free element. define some other threshold.
#include "memlab.h"

Main_Memory* memory = NULL;
LocalTable* local_table = NULL;
Stack* gc_stack = NULL;

bool Stack::push(int x)
{
    if (top >= (MAX_STACK_SIZE - 1))
    {
        return false;
    }
    else 
    {
        a[++top] = x;
        return true;
    }
}
 
int Stack::pop()
{
    if (top < 0) {
        return 0;
    }
    else {
        int x = a[top--];
        return x;
    }
}
int Stack::first_elem()
{
    if (top < 0) {
        return 0;
    }
    else {
        int x = a[top];
        return x;
    }
}
 
bool Stack::isEmpty()
{
    return (top < 0);
}

int size_of_datatype(int var_type) // returns in bits
{
	int datatype_sizes[4] = {32,8,24,1};

	return datatype_sizes[var_type];
}

int LocalTable::alloc(unsigned int wordidx, unsigned int offset) {
    if (size == MAX_VARS) {
        return -1;
    }
    unsigned int idx = head;
    head = (local_array[head].offset & -2) >> 1;
    local_array[idx].local_index = (wordidx << 1) | 1;  // mark as allocated
    local_array[idx].offset = (offset << 1) | 1;   // mark as in use
    size++;
   // LOG("LocalTable", _COLOR_BLUE, "Alloc symbol: %d at address: %d\n", idx, (wordidx << 2) | offset);
    return idx;  // local address
}

int LocalTable::getWordIdx(unsigned int idx)
{ 
    return local_array[idx].local_index >> 1; 
}
int LocalTable::getOffset(unsigned int idx) 
{
 return local_array[idx].offset >> 1; 
}
void LocalTable::setMarked(unsigned int idx) 
{ 
    local_array[idx].offset |= 1; 
}     
void LocalTable::setUnmarked(unsigned int idx) 
{
    local_array[idx].offset &= -2; 
}  
void LocalTable::setAllocated(unsigned int idx) 
{
local_array[idx].local_index |= 1; 
}  
void LocalTable::setUnallocated(unsigned int idx) 
{
    local_array[idx].local_index &= -2; 
}
bool LocalTable::isMarked(unsigned int idx) 
{ 
    return local_array[idx].offset & 1; 
}
bool LocalTable::isAllocated(unsigned int idx) 
{
    return local_array[idx].local_index & 1; 
}

void LocalTable::free(unsigned int idx) {
    unsigned int wordidx = getWordIdx(idx);
    unsigned int offset = getOffset(idx);
    if (size == MAX_VARS) {
        head = tail = idx;
        local_array[idx].local_index = 0;
        local_array[idx].offset = -2;  // sentinel
        size--;
        return;
    }
    local_array[tail].offset = idx << 1;
    local_array[idx].local_index = 0;
    local_array[idx].offset = -2;  // sentinel
    tail = idx;
    size--;
   // LOG("LocalTable", _COLOR_BLUE, "Freed symbol: %d at address: %d\n", idx, (wordidx << 2) | offset);
}

 int* LocalTable::getPtr(unsigned int idx)
 {
 	unsigned int wordidx = getWordIdx(idx);
 	int* p = (memory->start_mem + (wordidx)); 
  	return p;
 }

void start_scope()
{
	gc_stack->push(-1);
	return;
}

void end_scope()
{
	while(gc_stack->first_elem()!=-1)
	{
		local_table->setUnmarked(gc_stack->first_elem());
		int val = gc_stack->pop();
		if(val==-1)
		{
			gc_stack->pop();
			break;
		}
	}
}

void gc_run() {
    for (int i = 0; i < MAX_VARS; i++) {
        pthread_mutex_lock(&memory->memory_mutex);
        pthread_mutex_lock(&local_table->localtable_mutex);
        if (local_table->isAllocated(i) && !local_table->isMarked(i)) 
        {
            freeElem(i);
            compactMem();
        }
        pthread_mutex_unlock(&local_table->localtable_mutex);
        pthread_mutex_unlock(&memory->memory_mutex);
    }
}

void handlSigUSR1(int sig) {
    gc_run();
}


void gc_initialize()
{
	 signal(SIGUSR1, handlSigUSR1);
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    while (true) {
        usleep(10 * 1000);
        pthread_sigmask(SIG_BLOCK, &set, NULL);
        gc_run();
        pthread_sigmask(SIG_UNBLOCK, &set, NULL);
    }
}

void* garbageCollector(void*) 
{
	gc_initialize();   
}

void calcOffset() {
    int* p = memory->start_mem;
    int offset = 0;
    while (p < memory->end_mem) {
        if ((*p & 1) == 0) {
            offset += (*p >> 1);
        } else {
            *(p + (*p >> 1) - 1) = (((p - offset) - memory->start_mem) << 1) | 1;
        }
        p = p + (*p >> 1);
    }
}

void updateSymbolTable() {
    for (int i = 0; i < MAX_VARS; i++) {
        if (local_table->isAllocated(i)) {
            int* p = local_table->getPtr(i) - 1;
            int newWordId = *(p + (*p >> 1) - 1) >> 1;
            local_table->local_array[i].local_index = (newWordId << 1) | 1;
        }
    }
}

void compactMem() {
    calcOffset();
    updateSymbolTable();
    int* p = memory->start_mem;
    int* next = p + (*p >> 1);
    while (next != memory->end_mem) {
        if ((*p & 1) == 0 && (*next & 1) == 1) {
            int word1 = *p >> 1;
            int word2 = *next >> 1;
            memcpy(p, next, word2 << 2);
            p = p + word2;
            *p = word1 << 1;
            *(p + word1 - 1) = word1 << 1;
            next = p + word1;
            if (next != memory->end_mem && (*next & 1) == 0) {
                word1 = word1 + (*next >> 1);
                *p = word1 << 1;
                *(p + word1 - 1) = word1 << 1;
                next = p + word1;
            }
        } else {
            p = next;
            next = p + (*p >> 1);
        }
    }
    p = memory->start_mem;
    while (p < memory->end_mem) {
        *(p + (*p >> 1) - 1) = *p;
        p = p + (*p >> 1);
    }
}

void createMem(int mem_size)
{
	void *p = malloc(mem_size); //malloc takes mem in bytes
	if(p==NULL)
	{
		perror("malloc");
		exit(0);
	}
	
	memory = new Main_Memory(p,mem_size);
	local_table = new LocalTable();
	gc_stack = new Stack();
	 pthread_t gc_thread;
	 pthread_attr_t attr;
     pthread_attr_init(&attr);
     pthread_create(&gc_thread, &attr, garbageCollector, NULL);
	return ;
}

int Main_Memory::get_memory(int size) { // size in bytes (4 bytes aligned)
    int* p = start_mem;

    int newsize = (((size + 3) >> 2) << 2) + 8;
    while ((p < end_mem) &&
           ((*p & 1) ||
            ((*p << 1) < newsize)))
        p = p + (*p >> 1);
    if (p == end_mem) {
        return -1;
    }
    allocate_memory((int*)p, newsize);
    return (p - start_mem);
}

void Main_Memory::allocate_memory(int* ptr, int size) {
    int oldsize = *ptr << 1; // old size in bytes
    int words = size >> 2;
    *ptr = (words << 1) | 1;
    *(ptr + words - 1) = (words << 1) | 1; // footer
    cout<<oldsize<<endl;
    if (size < oldsize) {
        *(ptr + words) = (oldsize - size) >> 1;
        *(ptr + (oldsize >> 2) - 1) = (oldsize - size) >> 1;
    }
}

Datatype createVar(int var_type)
{
	
	int size = (size_of_datatype(var_type))/4; // size is in bits
	int wordId = memory->get_memory(size);// Find free block
	cout<<wordId<<endl;
	int local_index = local_table->alloc(wordId,0);
	local_table->setAllocated(local_index);
	local_table->setMarked(local_index);
	gc_stack->push(local_index);
	return Datatype(local_index,var_type,false);
 }

void assignVar(Datatype d,int value)
{
	if(d.var_type!=0)
	{
		cout<<"Please give integer type value"<<endl;
		return;
	}
	int local_index = d.local_index;
	int* pt = local_table->getPtr(local_index);
 	memcpy(pt,&value,4);
}

// void assignVar(Datatype d,medium_int m)

void assignVar(Datatype d,char c)
{
	if(d.var_type!=1)
	{
		cout<<"Please give character type value"<<endl;
		return;
	}
	int local_index = d.local_index;
	int* pt = local_table->getPtr(local_index);
	int value = c;
 	memcpy(pt,&value,4);	
}

void assignVar(Datatype d,bool b)
{
	if(d.var_type!=3)
	{
		cout<<"Please give bool type value"<<endl;
		return;
	}
	int local_index = d.local_index;
	int* pt = local_table->getPtr(local_index);
	int value = b;
 	memcpy(pt,&value,4);	
}

Datatype createArr(int arr_len,int var_type)
{
	int total_size;
	if(var_type==3)
		total_size = (arr_len*size_of_datatype(var_type))/8;
	else 
		total_size = arr_len*(size_of_datatype(var_type)/4);

	if(total_size%4!=0)
	{
		total_size = (total_size/4)*4 + 4;
	}

	int wordId = memory->get_memory(total_size);// Find free block
	cout<<wordId<<endl;
	int local_index = local_table->alloc(wordId,0);
	local_table->setAllocated(local_index);
	local_table->setMarked(local_index);
	gc_stack->push(local_index);

	return Datatype(local_index,var_type,true);
}

void assignArr(Datatype d,int arr_indx,int value) // Assign value at some index for integer array 
{
	if(d.var_type!=0)
	{
		cout<<"Please give int type value"<<endl;
		return;
	}
	int local_index = d.local_index;
	int* pt = local_table->getPtr(local_index);
	pt = pt + arr_indx;
 	memcpy(pt,&value,4);		
}

void assignArr(Datatype d,int arr_indx,char c) // Assign value at some index for char array 
{
	if(d.var_type!=1)
	{
		cout<<"Please give char type value"<<endl;
		return;
	}
	int local_index = d.local_index;
	int* pt = local_table->getPtr(local_index);
	memcpy(((char *)pt+arr_indx),&c,1);
}

// void assignArr(Datatype d,int arr_indx,med_int m) // Assign value at some index for medium int array 
// {

// }

void assignArr(Datatype d,int arr_indx,bool b) // Assign value at some index for bool array 
{

}

void assignArr(Datatype d,int arr[],int arr_size )  // Assign value to entire integer array
{
	if(d.var_type!=0)
	{
		cout<<"Please give int type value"<<endl;
		return;
	}
	int local_index = d.local_index;
	int* pt = local_table->getPtr(local_index);
	for (int i = 0; i < arr_size; ++i)
	{
		/* code */
		*(pt + i) = arr[i];
	}
}

void assignArr(Datatype d,char arr[] )  // Assign value to entire char array
{

}

void assignArr(Datatype d,medium_int arr[] )  // Assign value to entire medium int array
{

}

void assignArr(Datatype d,bool arr[] )  // Assign value to entire bool array
{

}
void freeElem(int loc_id)
{
	int* p = local_table->getPtr(loc_id);
	*p = (*p) & (-2);  // Clear allocated flag in last bit

	int* next_block = p + *p;
	if((*next_block & 1)==0)  // Check for next block
	{
		*p = *p + *next_block; 
	}

	// Need to check again
	// if (p != memory->start_mem && (*(p - 1) & 1) == 0)  // Check for prev block
	// {  
 //        int prev_size = (*(p - 1) >> 1);
 //        *(p - prev_size) = (prev_size + words) << 1;  // new size in words
 //        *(p + words - 1) = (prev_size + words) << 1;  // footer
 //        words = words + prev_size;
 //    }
}

int main(){
	int size ; 

	cout << "Enter Size : " << "\n" ; 
	cin >> size ; 


	createMem(size) ; 

	Datatype d = createVar(0);
	assignVar(d,5);

}
