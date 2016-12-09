#include "Memory.h"
#include "math.h"

void Memory::validateIndex(unsigned int index, unsigned int bytes)
{
	if (bytes > WORD_SIZE)
		throw std::out_of_range("Too many bytes!");
	if (index + bytes > BLOCK_SIZE)
		throw std::out_of_range("Bad index!");
}

Memory::Memory()
{
	for (int i = 0; i < BLOCK_SIZE; i++)
		memory[i] = 0;
}

Memory::~Memory()
{
}

int Memory::fetch(unsigned int index, unsigned int bytes)
{
	validateIndex(index, bytes);
	
	int retVal = 0;
	for (unsigned int i = 0; i < bytes; i++)
	{
		retVal <<= BYTE_SIZE;
		retVal += memory[index + i];
	}
	
	return retVal;
}

void Memory::store(unsigned int index, unsigned int value, unsigned int bytes)
{
	validateIndex(index, bytes);

	for (unsigned int i = 0; i < bytes; i++)
	{
		unsigned int shift = (bytes - 1 - i) * BYTE_SIZE;
		unsigned int temp = value >> shift ;	// Bitshift right by the correct number of bytes
		memory[index + i] = temp;
	}
}
