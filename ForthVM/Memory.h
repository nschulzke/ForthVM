#pragma once

#include <stdexcept>

typedef unsigned char byte;

class Memory
{
public:
	static const int BLOCK_SIZE = 65536;
	static const int BYTE_SIZE = 8;
	static const int WORD_SIZE = 4;
private:
	byte memory[BLOCK_SIZE];
	void validateIndex(unsigned int index, unsigned int bytes);
public:
	Memory();
	~Memory();
	int fetch(unsigned int index, unsigned int bytes = WORD_SIZE);
	void store(unsigned int index, unsigned int value, unsigned int bytes = WORD_SIZE);
};

