//Stack.h
#pragma once

template <typename T>
class Stack {
private:
	int size;  // Number of elements 
	int top;
	T* stackPtr;
public:
	Stack(int size = 10);
	~Stack() { delete[] stackPtr; }
	bool push(const T& item);
	bool pop(T& item);  // pop an element off the stack
	bool isEmpty() const { return (top == -1); }
	bool isFull() const { return (top == size - 1); }
};

//constructor with the default size 10
template <typename T>
Stack<T>::Stack(int s)
{
	size = s > 0 && s < 1000 ? s : 10;
	stackPtr = new T[size];
	top = -1;  // initialize stack
}

// push an element onto the Stack 
template <typename T>
bool Stack<T>::push(const T& item)
{
	if (isFull()) return false;  // push unsuccessful
	stackPtr[++top] = item;
	return true;  // push successful
}

// pop an element off the Stack
template <typename T>
bool Stack<T>::pop(T& popValue)
{
	if (isEmpty()) return false;  // pop unsuccessful
	popValue = stackPtr[top--];
	return true;  // pop successful
}
