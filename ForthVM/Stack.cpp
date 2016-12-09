#include "Stack.h"

Stack::Stack(int sizeIn)
{
	size = sizeIn;
	stack = new int[size];
	top = -1;
}

Stack::~Stack()
{
	delete[] stack;
}

void Stack::push(int value)
{
	if ((top + 1) < size)
		stack[++top] = value;
	else
		throw StackException("Stack overflow!");
}

int Stack::pop()
{
	if (top >= 0)
		return stack[top--];
	else
		throw StackException("Stack underflow!");
}

int Stack::depth() const
{
	return top + 1;	// Current depth of stack
}

int Stack::peek(int index) const
{
	if (index <= top)
		return stack[index];
	else
		throw StackException("Bad peek index!");
}

void Stack::clear()
{
	top = -1;
}