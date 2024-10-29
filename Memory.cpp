//Memroy Code
// By Grant Williams
#include "Memory.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

Memory::Memory(char* fName)
{
	ifstream in(fName);
	string str;
	//printf("The file used is %s\n", fName);
	int p = 0;
	while(!in.eof())
	{
		getline(in,str);
		int i = 0;
		for(i = 0; i<str.length();i++)
		{ if(isblank(str[i])) break;}
		if(ispunct(str[0]))
		{
			str = str.substr(1,i);
			p = atoi(str.c_str());
		}
		else if(!isblank(str[0]))
		{
			str = str.substr(0,i);
			mem[p] = atoi(str.c_str());
			printf("%i: %i \n",p , mem[p]);
			p++;
		}
		//p++;
	}
	//printf("1000: %i", mem[1000]);
	in.close();
}
int Memory::read(int addr)
{
	return mem[addr];
}
int Memory::write(int addr, int data)
{
	mem[addr] = data;
	return (addr - 1);
}

