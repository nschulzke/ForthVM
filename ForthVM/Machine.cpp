#include "Machine.h"

const uint64_t DOUBLE = 4294967296;

Machine::Machine()
{
	PC = 0;

	IR = 0;
	IP = 0;

	running = true;
}

Machine::~Machine()
{
}

void Machine::execute()
{
	byte instruction = memory.fetch(PC, 1);
	PC++;

	byte high = instruction & 0xF0;
	byte low = instruction & 0x0F;
	byte low3 = instruction & 0x0E;
	bool bit0 = instruction & 0x01;
	bool bit3 = instruction & 0x08;
	if (instruction == 0xFF)
	{
		running = false;
	}
	else if (high == 0x00)
	{	// 0x0X 0000 | PUSH
		if ((low & 0x08) == 0x00)
		{	// 0xxx | PUSH reg
			if (low == 0x01)		// 0x01 0001 | PUSH IP
				push(dataStack, IP);
			else if (low == 0x02)	// 0x02 0010 | PUSH DSP
				push(dataStack, dataStack.depth());
			else if (low == 0x03)	// 0x03 0011 | PUSH RSP
				push(dataStack, returnStack.depth());
		}
		else
		{	// 1xxx | PUSH s,#
			byte bytes = low & 0x06;
			// Bits n in xnnx specify number of bytes to read
			bytes = (bytes >> 1) + 1; // Bitshift right 1, add one
			// Bit 0 specifies which stack
			if (bit0 == false)	// PUSH D,#
				push(dataStack, signExtend(memory.fetch(PC, bytes), bytes));
			else				// PUSH R,#
				push(returnStack, signExtend(memory.fetch(PC, bytes), bytes));
			PC += bytes;
		}
	}
	else if (high == 0x10)
	{	// 0x1X 0001 | Stack ops
		if (low3 == 0x00)
		{
			if (bit0 == false)	// 0x10 0000 | TOd
				push(dataStack, pop(returnStack));
			else				// 0x11 0001 | dTOd
				pushD(dataStack, popD(returnStack));
		}
		else if (low3 == 0x02)
		{
			if (bit0 == false)	// 0x12 0010 | TOr
				push(returnStack, pop(dataStack));
			else				// 0x13 0011 | dTOr
				pushD(returnStack, popD(dataStack));
		}
		else if (low3 == 0x04)
		{
			if (bit0 == false)	// 0x14 0100 | DROP
				pop(dataStack);
			else				// 0x15 0101 | dDROP
				popD(dataStack);
		}
		else if (low3 == 0x06)
		{
			if (bit0 == false)	// 0x16 0110 | DUP
			{
				unsigned int num = pop(dataStack);
				push(dataStack, num);
				push(dataStack, num);
			}
			else				// 0x17 0111 | dDUP
			{
				uint64_t num = popD(dataStack);
				pushD(dataStack, num);
				pushD(dataStack, num);
			}
		}
		else if (low3 == 0x08)
		{
			if (bit0 == false)	// 0x18 1000 | SWAP
			{
				unsigned int num2 = pop(dataStack);
				unsigned int num1 = pop(dataStack);
				push(dataStack, num2);
				push(dataStack, num1);
			}
			else				// 0x19 1001 | dSWAP
			{
				uint64_t num2 = popD(dataStack);
				uint64_t num1 = popD(dataStack);
				pushD(dataStack, num2);
				pushD(dataStack, num1);
			}
		}
		else if (low == 0x0A)
		{	// 0x1A 1010 | ROT
			unsigned int num3 = pop(dataStack);
			unsigned int num2 = pop(dataStack);
			unsigned int num1 = pop(dataStack);
			push(dataStack, num2);
			push(dataStack, num3);
			push(dataStack, num1);
		}
	}
	else if (high == 0x20)
	{	// 0010 | Arithmetic
		if (low3 == 0x00)
		{	// 000x | ADD / dADD
			add(bit0);		// If bit0 = 1, then double
		}
		else if (low3 == 0x02)
		{	// 001x | SUB / dSUB
			sub(bit0);		// If bit0 = 1, then double
		}
		else if (low3 == 0x04)
		{	// 010x | MULT / dMULT
			mult(bit0);		// If bit0 = 1, then double
		}
		else if (low3 == 0x06)
		{	// 011x | DIVMOD / dDIVMOD
			divmod(bit0);	// If bit0 = 1, then double
		}
		else if (low3 == 0x08)
		{	// 100x | AND / dAND
			and(bit0);		// If bit0 = 1, then double
		}
		else if (low3 == 0x0A)
		{	// 101x | OR / dOR
			or(bit0);		// If bit0 = 1, then double
		}
		else if (low3 == 0x0C)
		{	// 110x | XOR / dXOR
			xor(bit0);		// If bit0 = 1, then double
		}
		else if (low3 == 0x0E)
		{	// 110x | NOT / dNOT
			not(bit0);		// If bit0 = 1, then double
		}
	}
	else if (high == 0x30)
	{
		if (bit3 == false)
		{	// 0nzp | BRnzp
			unsigned int branchTo = signExtend(memory.fetch(PC++, 1), 1);
			int toTest = dataStack.pop();
			bool n = (low & 0x4) && (toTest < 0);
			bool z = (low & 0x2) && (toTest == 0);
			bool p = (low & 0x1) && (toTest > 0);
			if (n | z | p)
				PC += branchTo;
		}
		else
		{
			unsigned int num1 = 0;
			unsigned int num2 = 0;
			uint64_t dnum1 = 0;
			uint64_t dnum2 = 0;
			if (bit0 == false)
			{
				num2 = pop(dataStack);
				num1 = pop(dataStack);
			}
			else
			{
				dnum2 = popD(dataStack);
				dnum1 = popD(dataStack);
			}
			if (low3 == 0x08)
			{
				if (bit0 == false)
					push(dataStack, boolean(num1 == num2));
				else
					push(dataStack, boolean(dnum1 == dnum2));
			}
			else if (low3 == 0x0A)
			{
				if (bit0 == false)
					push(dataStack, boolean(num1 < num2));
				else
					push(dataStack, boolean(dnum1 < dnum2));
			}
			else if (low3 == 0x0C)
			{
				if (bit0 == false)
					push(dataStack, boolean(num1 > num2));
				else
					push(dataStack, boolean(dnum1 > dnum2));
			}
		}
	}
}

int Machine::signExtend(int valIn, unsigned int bytes)
{
	int byte = 0x1 << (bytes * memory.BYTE_SIZE - 1);
	bool negative = (0 != (valIn & byte));
	if (negative)
	{
		int maskBytes = memory.WORD_SIZE - bytes;
		int mask = 0xFF << memory.BYTE_SIZE * (memory.WORD_SIZE - 1);
		for (int i = 0; i < maskBytes; i++)
		{
			valIn |= mask;
			mask >>= memory.BYTE_SIZE;
		}
	}
	return valIn;
}

int Machine::boolean(bool valIn)
{
	if (valIn == true)
		return -1;
	else
		return 0;
}

void Machine::run()
{
	while (running)
		execute();
}

void Machine::arrayToMem(char array[], unsigned int size)
{
	for (unsigned int i = 0; i < size; i++)
		memory.store(i, array[i], 1);
}

void Machine::dumpStacks()
{
	std::cout << "  Data:";
	for (int i = 0; i < dataStack.depth(); i++)
		std::cout << " " << dataStack.peek(i);
	std::cout << std::endl;

	std::cout << "Return:";
	for (int i = 0; i < returnStack.depth(); i++)
		std::cout << " " << returnStack.peek(i);
	std::cout << std::endl;
}

unsigned int Machine::pop(Stack& stack)
{
	return stack.pop();
}

uint64_t Machine::popD(Stack& stack)
{
	uint64_t high = stack.pop();
	uint64_t low = stack.pop();
	return high * DOUBLE + low;
}

void Machine::push(Stack& stack, unsigned int val)
{
	stack.push(val);
}

void Machine::pushD(Stack& stack, uint64_t val)
{
	int high = val / DOUBLE;
	int low = val;
	stack.push(low);
	stack.push(high);
}

void Machine::store(bool dbl)
{
	unsigned int addr = pop(dataStack);
	if (dbl == false)
	{
		unsigned int val = pop(dataStack);
		memory.store(addr, val);
	}
	else
	{
		unsigned int high = pop(dataStack);
		unsigned int low = pop(dataStack);
		memory.store(addr, high);
		memory.store(addr + memory.WORD_SIZE, low);
	}
}

void Machine::fetch(bool dbl)
{
	unsigned int addr = pop(dataStack);
	if (dbl == true)
		push(dataStack, memory.fetch(addr + memory.WORD_SIZE));
	push(dataStack, memory.fetch(addr));
}

void Machine::add(bool dbl)
{
	if (dbl == false)
	{
		unsigned int num2 = pop(dataStack);
		unsigned int num1 = pop(dataStack);
		push(dataStack, num1 + num2);
	}
	else
	{
		uint64_t num2 = popD(dataStack);
		uint64_t num1 = popD(dataStack);
		pushD(dataStack, num1 + num2);
	}
}

void Machine::sub(bool dbl)
{
	if (dbl == false)
	{
		unsigned int num2 = pop(dataStack);
		unsigned int num1 = pop(dataStack);
		push(dataStack, num1 - num2);
	}
	else
	{
		uint64_t num2 = popD(dataStack);
		uint64_t num1 = popD(dataStack);
		pushD(dataStack, num1 - num2);
	}
}

void Machine::mult(bool dbl)
{
	if (dbl == false)
	{
		unsigned int num2 = pop(dataStack);
		unsigned int num1 = pop(dataStack);
		push(dataStack, num1 * num2);
	}
	else
	{
		uint64_t num2 = popD(dataStack);
		uint64_t num1 = popD(dataStack);
		pushD(dataStack, num1 * num2);
	}
}

void Machine::divmod(bool dbl)
{
	if (dbl == false)
	{
		unsigned int num2 = pop(dataStack);
		unsigned int num1 = pop(dataStack);
		push(dataStack, num1 % num2);
		push(dataStack, num1 / num2);
	}
	else
	{
		uint64_t num2 = popD(dataStack);
		uint64_t num1 = popD(dataStack);
		pushD(dataStack, num1 % num2);
		pushD(dataStack, num1 / num2);
	}
}

void Machine::and(bool dbl)
{
	if (dbl == false)
	{
		unsigned int num2 = pop(dataStack);
		unsigned int num1 = pop(dataStack);
		push(dataStack, num1 & num2);
	}
	else
	{
		uint64_t num2 = popD(dataStack);
		uint64_t num1 = popD(dataStack);
		pushD(dataStack, num1 & num2);
	}
}

void Machine::or(bool dbl)
{
	if (dbl == false)
	{
		unsigned int num2 = pop(dataStack);
		unsigned int num1 = pop(dataStack);
		push(dataStack, num1 | num2);
	}
	else
	{
		uint64_t num2 = popD(dataStack);
		uint64_t num1 = popD(dataStack);
		pushD(dataStack, num1 | num2);
	}
}

void Machine::xor(bool dbl)
{
	if (dbl == false)
	{
		unsigned int num2 = pop(dataStack);
		unsigned int num1 = pop(dataStack);
		push(dataStack, num1 ^ num2);
	}
	else
	{
		uint64_t num2 = popD(dataStack);
		uint64_t num1 = popD(dataStack);
		pushD(dataStack, num1 ^ num2);
	}
}

void Machine::not(bool dbl)
{
	if (dbl == false)
	{
		unsigned int num = pop(dataStack);
		push(dataStack, ~num);
	}
	else
	{
		uint64_t num = popD(dataStack);
		pushD(dataStack, ~num);
	}
}

void Machine::next()
{
	IR = memory.fetch(IP);
	IP++;
	PC = IR;
}
