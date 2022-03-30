// Name: Aryan Mehta
// Roll No.: 19CS30006

// Name: Pranav Rajput
// Roll no.: 19CS30036

// Assignment: Assignment 5
// Date: 22.03.2022
// Group: 5

#ifndef __MEMLAB_H
#define __MEMLAB_H

#include<iostream>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>  
#include <stdlib.h>  
#include <fcntl.h>  
#include <signal.h>  
#include <sys/types.h> 
using namespace std;


#define NUM_VAR_TYPES 4
#define MAX_VARS     1000
#define MAX_STACK_SIZE 1000
// Variable types: 
// 0 - int
// 1 - char 
// 2 - med int
// 3 - bool

struct medium_int
{
	char val[3];
};
struct Main_Memory
{
	int* start_mem;
	int* end_mem;
	// add lock
	pthread_mutex_t memory_mutex;
	Main_Memory(void* p,int mem_size)
	{
		start_mem = (int *)p;
		*start_mem = mem_size << 1;
		end_mem = start_mem + mem_size;
	}
	int get_memory(int size);

	void allocate_memory(int* ptr, int size);

	void free_memory(int* ptr);
} ;

struct Stack {
    int top; 
    int a[MAX_STACK_SIZE]; // Maximum size of Stack
 
    Stack() { top = -1; }
    bool push(int x);
    int pop();
    int first_elem();
    bool isEmpty();
};
 

struct Variable {
    unsigned int local_index;
    unsigned int offset;
};

struct LocalTable {
    unsigned int head, tail;
    Variable local_array[MAX_VARS];
    int size;
    pthread_mutex_t localtable_mutex;
    int alloc(unsigned int wordidx, unsigned int offset);
    void free(unsigned int idx);
    int getWordIdx(unsigned int idx);
    int getOffset(unsigned int idx);
    void setMarked(unsigned int idx);     // mark as in use
    void setUnmarked(unsigned int idx);  // mark as free
    void setAllocated(unsigned int idx);  // mark as allocated
    void setUnallocated(unsigned int idx);
    bool isMarked(unsigned int idx);
    bool isAllocated(unsigned int idx);
    int* getPtr(unsigned int idx);
};
struct Datatype
{
	int local_index;
	int var_type;
	bool isArray;

	Datatype(int l, int v,bool iA)
	{
		local_index = l;
		var_type = v;
		isArray = iA;
	}
};

void start_scope();

void end_scope();

void gc_run();
void handlSigUSR1(int sig);
void gc_initialize();

void calcOffset();
void updateSymbolTable();

void compactMem();

int size_of_datatype(int var_type);

void createMem(int mem_size); // mem_size is number of bytes here

Datatype createVar(int var_type);

void assignVar(Datatype d,int value);
// void assignVar(Datatype d,medium_int m);
void assignVar(Datatype d,char c);
void assignVar(Datatype d,bool b);

Datatype createArr(int arr_len,int var_type);

void assignArr(Datatype d,int arr_indx,int value); // Assign value at some index for integer array 
void assignArr(Datatype d,int arr_indx,char c); // Assign value at some index for char array 
void assignArr(Datatype d,int arr_indx,med_int m); // Assign value at some index for medium int array 
void assignArr(Datatype d,int arr_indx,bool b); // Assign value at some index for bool array 
void assignArr(Datatype d,int arr[],int arr_size ); // Assign value to entire integer array
void assignArr(Datatype d,char arr[] ); // Assign value to entire char array
void assignArr(Datatype d,medium_int arr[] ); // Assign value to entire medium int array
void assignArr(Datatype d,bool arr[] );  // Assign value to entire bool array

void freeElem(int loc_id);
#endif // __MEMLAB_H
