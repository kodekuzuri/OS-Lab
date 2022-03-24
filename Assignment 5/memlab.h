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

struct block
{
	int size;
	int padding;
	int free_flag;
	void *data;

};
void* createMem(int memory); // memory is number of bytes here

void createVar(int var_size,int type_of_var);

void assignVar(int value);

void createArr(int arr_size,int datatype)

// assign array

// freeElem

#endif // __MEMLAB_H
