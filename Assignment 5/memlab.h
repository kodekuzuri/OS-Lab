// Name: Aryan Mehta
// Roll No.: 19CS30006

// Name: Pranav Rajput
// Roll no.: 19CS30036

// Assignment: Assignment 5
// Date: 22.03.2022
// Group: 5

#ifndef __MEMLAB_H
#define __MEMLAB_H

#include<bits/stdc++.h>
using namespace std;


#define NUM_VAR_TYPES 4
// Variable types: 
// 0 - int
// 1 - char 
// 2 - med int
// 3 - bool

struct block
{
	int size;
	int padding;
	int free_flag;
	void *data;
	block *next;

	int getMemory(int n){
		
	}


};


struct Datatype
{
	int local_index;
	int var_type;

	Datatype(int l, int v)
	{
		local_index = l;
		var_type = v;
	}
};

int size_of_datatype(int var_type);

void createMem(int memory); // memory is number of bytes here

Datatype createVar(int var_type);

void assignVar(int value);

void createArr(int arr_size,int datatype);

// assign array

// freeElem

#endif // __MEMLAB_H
