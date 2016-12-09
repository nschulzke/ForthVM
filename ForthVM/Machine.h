#pragma once

#include "Memory.h"
#include "Stack.h"
#include <stdint.h>
#include <iostream>

class Machine
{
private:
	Memory memory;
	Stack callStack;
	
	int PC;				// Program counter -- bytecode instructions to be executed
	
	int IP;				// Forth interpreter pointer, holds the next Forth codeword pointer's address
	int IR;				// Forth interpreter register, holds the current Forth codeword pointer (to be jumped to indirectly)

	bool running;
public:
	Stack dataStack;
	Stack returnStack;
	Machine();
	~Machine();
	
	int signExtend(int valIn, unsigned int bytes);
	int boolean(bool valIn);

	void run();
	void execute();
	void arrayToMem(char array[], unsigned int size);
	void dumpStacks();
	
	// stack: ( val -- )
	// Returns the top of the stack, removes that item
	unsigned int pop(Stack& stack);
	// stack: ( dval -- )
	// Returns the top two items of the stack as a double, removes them
	uint64_t popD(Stack& stack);

	// stack: ( -- val )
	// Push val to stack
	void push(Stack& stack, unsigned int val);
	// stack: ( -- dval )
	// Push double to stack
	void pushD(Stack& stack, uint64_t val);

	// data: ( val addr -- )
	// Stores val in memory location addr
	void store(bool dbl = false);
	// data: ( addr -- val )
	// Push data at addr to data stack
	void fetch(bool dbl = false);
	
	// data: ( a b -- a+b )
	// Adds the top two values on the data stack
	void add(bool dbl = false);
	// data: ( a b -- a-b )
	// Subtracts the top two values on the data stack
	void sub(bool dbl = false);
	// data: ( a b -- a*b )
	// Multiplies the top two values on the data stack
	void mult(bool dbl = false);
	// data: ( a b -- a%b a/b )
	// Divides the top two values on the data stack. Pushes back first remainder, then quotient.
	void divmod(bool dbl = false);
	
	// data: ( a b -- a&b )
	// Bitwise AND on the top two values on the data stack
	void and(bool dbl = false);
	// data: ( a b -- a|b )
	// Bitwise OR on the top two values on the data stack
	void or(bool dbl = false);
	// data: ( a b -- xor(a,b) )
	// Bitwise XOR on the top two values on the data stack.
	void xor(bool dbl = false);
	// Bitwise NOT on the top two values on the data stack.
	void not(bool dbl = false);
	
	// IR = IP, then IP++, then jump(IR);
	// Moves IP to the next location, then jumps to its old location.
	void next();
};
