#pragma once

#include <string>

const int DEFAULT_SIZE = 128;

class Stack
{
private:
	int top;
	int* stack;
	int size;
public:
	Stack(int sizeIn = DEFAULT_SIZE);
	~Stack();
	void push(int value);
	int pop();
	int depth() const;
	int peek(int index) const;
	void clear();
};

class StackException
{
private:
	std::string exception;
public:
	StackException(std::string exceptionIn)
		:exception(exceptionIn) {}
	std::string what() { return exception; }
};
