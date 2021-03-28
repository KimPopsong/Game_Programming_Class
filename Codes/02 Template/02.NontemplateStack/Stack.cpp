//Stack.cpp
#include "stdafx.h"
#include "Stack.h"

//constructor with the default size 10
Stack::Stack(int s)
{
	size = s > 0 && s < 1000 ? s : 10;
	stackPtr = new int[size];
	top = -1;  // initialize stack
}

// push an element onto the Stack 
bool Stack::push(const int& item)
{
	if (isFull()) return false;  // push unsuccessful
	stackPtr[++top] = item;
	return true;  // push successful
}

// pop an element off the Stack
bool Stack::pop(int& popValue)
{
	if (isEmpty()) return false;  // pop unsuccessful
	popValue = stackPtr[top--];
	return true;  // pop successful
}
