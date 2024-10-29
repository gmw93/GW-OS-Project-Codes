#ifndef MEMORY_H
#define MEMORY_H
class Memory
{
	public:
		Memory(char* fName);
		int read(int addr);
		int write(int addr, int data);
		int mem[2000];
};

#endif
