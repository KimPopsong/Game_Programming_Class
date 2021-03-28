//Stack.h
#pragma once

class Stack {
private:
	int size;  // Number of elements 
	int top;
	int* stackPtr;
public:
	Stack(int size = 10);
	~Stack() { delete[] stackPtr; }
	bool push(const int& item);
	bool pop(int& item);  // pop an element off the stack
	bool isEmpty() const { return (top == -1); }
	bool isFull() const { return (top == size - 1); }
};

