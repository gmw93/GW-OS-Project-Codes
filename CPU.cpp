//CPU Program 
// by Grant Williams
#include "Memory.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <ctime>

using namespace std;

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		return 1;
	}
	//printf("%s\n", *argv+8);
	//Piping is done here, the two processes represent memory and CPU
	int pd1[2], pd2[2];
	int p1, p2, f;

	p1 = pipe(pd1);//CPU -> Memory
	p2 = pipe(pd2);//Memory -> CPU

	f = fork();
	

	if(f == 0)//child reads from memory
	{
		Memory m(argv[1]);
		int req, data;
		read(pd1[0], &req, sizeof(req));
		while(req != -1)
		{
			//printf("%i", req);
			/*req = 1 means that it takes data out of the array baed on address sent
			 *req = 2 means that it takes data being sent and adds it to memory*/
			if(req == 1)
			{
				read(pd1[0], &req, sizeof(req)); //address
				req = m.read(req);//data requested = m.read (address of data)
				write(pd2[1], &req, sizeof(req));
			}
			else if(req == 2)
			{
				read(pd1[0], &req, sizeof(req));   //address in the array
				read(pd1[0], &data, sizeof(data)); //data being requested
				m.write(req, data);
			}
			read(pd1[0], &req, sizeof(req));
		}

		_exit(0);
	}
	else
	{
		int PC = 0;
		int SP = 999; 
		int IR = 0;
		int AC, X, Y;
		int req = 1;
		int val = 0;
		int iTimer = 0;
		bool sMode = false;
		int stop = atoi(argv[2]);
		
		while(IR != 50)
		{
			if(iTimer >= stop && !sMode)
			{
				iTimer = iTimer - 30;
				IR = 29;
				sMode = true;
			}
			else
			{
				write(pd1[1], &req, sizeof(req));//This requests the instruction for this step
				write(pd1[1], &PC, sizeof(PC));  //This is the address of the data requested
				PC++;
				read(pd2[0], &IR, sizeof(IR));
			}
			//else if(iTimer >= stop && !sMode)
			//{
			//	//printf("\nPerform System Call, %i instructions have passed\n", stop);
			//	iTimer = iTimer - 30;
			//	IR = 29;
			//	sMode = true;
			//}

			if(IR == 1 || IR == 2 || IR == 3 || IR == 4 || IR == 5 || IR == 7 || 
				IR == 9 || IR == 20 || IR == 21 || IR == 22 || IR == 23)
			{
				req = 1;
				write(pd1[1], &req, sizeof(req));
				write(pd1[1], &PC, sizeof(PC));
				PC++;	
				switch(IR)
				{
					case 1: //load value next 
						read(pd2[0], &AC, sizeof(AC));
						break;
					case 2: //load value at address
						read(pd2[0], &val, sizeof(val));
						req = 1;
						if(val > 999 && !sMode)
						{
							IR = 50;
							printf("Error Message");
							break;
						}
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &val, sizeof(val));
						read(pd2[0], &AC, sizeof(AC));
						break;
					case 3: //load value from address found in the given address
						read(pd2[0], &val, sizeof(val));
						req = 1;
						if(val > 999 && !sMode)
						{
							IR = 50;
							printf("Error Message");
							break;
						}
						for(int i = 0; i <= 1; i++)
						{
							write(pd1[1], &req, sizeof(req));
							write(pd1[1], &val, sizeof(val));
							read(pd2[0], &val, sizeof(val));
							if(i == 0 && val > 999 && !sMode)
							{
								IR = 50;
								printf("Error Message");
								i = 2;
							}
						}
						AC = val;
						break;
					case 4: //Load value at address + X
						read(pd2[0], &val, sizeof(val));
						val = val + X;
						if(val > 999 && !sMode)
						{
							IR = 50;
							printf("Error Message");
							break;
						}
						req = 1;
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &val, sizeof(val));
						read(pd2[0], &AC, sizeof(AC));
						break;
					case 5: //Load value at address + Y
						read(pd2[0], &val, sizeof(val));
						val = val + Y;
						if(val > 999 && !sMode)
						{
							IR = 50;
							printf("Error Message");
							break;
						}
						req = 1;
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &val, sizeof(val));
						read(pd2[0], &AC, sizeof(AC));
						//printf("addr + y = %i", AC);
						break;
					case 7: //Store AC at address
						read(pd2[0], &val, sizeof(AC));
						req = 2;
						if(val > 999 && !sMode)
						{
							IR = 50;
							printf("Errr Message");
							break;
						}
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &val, sizeof(val));
						write(pd1[1], &AC, sizeof(AC));
						break;
					case 9: //Write Character or Integer
						read(pd2[0], &val, sizeof(val));
						if(val == 1)
						{
							printf("%i", AC);
						}
						else
						{
							printf("%c", (char)AC);
						}
						break;
					case 20://Jump to address
						read(pd2[0], &PC, sizeof(PC));
						if(PC > 999 && !sMode)
						{
							IR = 50;
							printf("Error Message");
							break;
						}
						break;
					case 21://Jump to address if AC == 0
						read(pd2[0], &val, sizeof(val));
						if(val > 999 && !sMode)
						{
							IR = 50;
							printf("Error Message");
							break;
						}
						if(AC == 0)
						{
							PC = val;
						}
						break;
					case 22://Jump to address if AC != 0
						read(pd2[0], &val, sizeof(val));
						if(val > 999 && !sMode)
						{
							IR = 50;
							printf("Error Message");
							break;
						}
						if(AC != 0)
						{
							PC = val;
						}
						break;
					case 23://push return address to stack, jump to address
						read(pd2[0], &val, sizeof(val));
						if(val > 999 && !sMode)
						{
							IR = 50;
							printf("Error Message");
							break;
						}
						req = 2;
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &SP, sizeof(SP));
						SP--;
						write(pd1[1], &PC, sizeof(PC));
						PC = val;
						break;
					default: break;

				}
			}
			else
			{
				switch(IR)
				{
					case 0:
						printf("0");
						break;
					case 6://load value from SP + X into AC
						req = 1;
						write(pd1[1], &req, sizeof(req));
						val = SP + X + 1;
						if(val > 999 && !sMode)
						{
							IR = 50;
							printf("Error Message");
							break;
						}
						write(pd1[1], &val, sizeof(val));
						read(pd2[0], &AC, sizeof(AC));
						break;
					case 8: //Generates a random number between 1 and 100
						//srand(time(NULL));
						AC = rand() %100 + 1;
						break;
					case 10: // add X to AC
						AC = AC + X;
						break;
					case 11: // add Y to AC
						AC = AC + Y;
						break;
					case 12: // subtract X from AC
						AC = AC - X;
						break;
					case 13: // subtract Y from AC
						AC = AC - Y;
						break;
					case 14: // Copy AC to X
						X = AC;
						break;
					case 15: // Copy X to AC
						AC = X; 
						break;
					case 16: // Copy AC to Y
						Y = AC;
						break;
					case 17: // Copy Y to AC
						AC = Y;
						break;
					case 18: // Copy AC to SP
						SP = AC;
						break;
					case 19: // Copy SP to AC
						AC = SP;
						break;
					case 24:// Pop Return address and jump to the address
						req = 1;
						write(pd1[1], &req, sizeof(req));
						SP++;
						write(pd1[1], &SP, sizeof(SP));
						read(pd2[0], &PC, sizeof(PC));
						break;
					case 25: // Increment X
						X++;
						break;
					case 26: // Decrement X
						X--;
						break;
					case 27: // Push AC into Stack
						req = 2;
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &SP, sizeof(SP));
						SP--;
						write(pd1[1], &AC, sizeof(AC));
						break;
					case 28: // Pop AC from Stack
						req = 1;
						write(pd1[1], &req, sizeof(req));
						SP++;
						write(pd1[1], &SP, sizeof(SP));
						read(pd2[0], &AC, sizeof(AC));
						break;
					case 29: //Performs System Call
						req = 2;
						val = 1999;
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &val, sizeof(val));
						write(pd1[1], &SP, sizeof(SP));
						SP = val - 1;
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &SP, sizeof(SP));
						write(pd1[1], &PC, sizeof(PC));
						if(sMode)
						{ PC = 1000; }
						else
						{ 
							PC = 1500;
							sMode = true;
							//printf("Wrong World");
						}
						SP--;
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &SP, sizeof(SP));
						write(pd1[1], &AC, sizeof(AC));
						SP--;
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &SP, sizeof(SP));
						write(pd1[1], &X, sizeof(X));
						SP--;
						write(pd1[1], &req, sizeof(req));
						write(pd1[1], &SP, sizeof(SP));
						write(pd1[1], &Y, sizeof(Y));
						SP--;
						
						break;
					case 30://Returns from System Call
						req = 1;
						//pop the Y value
						write(pd1[1], &req, sizeof(req));
						SP++;
						write(pd1[1], &SP, sizeof(SP));
						read(pd2[0], &Y, sizeof(Y));
						//pop the X value
						write(pd1[1], &req, sizeof(req));
						SP++; 
						write(pd1[1], &SP, sizeof(SP));
						read(pd2[0], &X, sizeof(X));
						//pop the AC
						write(pd1[1], &req, sizeof(req));
						SP++;
						write(pd1[1], &SP, sizeof(SP));
						read(pd2[0], &AC, sizeof(AC));
						//pop the PC
						write(pd1[1], &req, sizeof(req));
						SP++;
						write(pd1[1], &SP, sizeof(SP));
						read(pd2[0], &PC, sizeof(PC));
						//pop the SP
						write(pd1[1], &req, sizeof(req));
						SP++;
						write(pd1[1], &SP, sizeof(SP));
						read(pd2[0], &SP, sizeof(SP));
						break;
					default:
						break;
				}
			}
			iTimer++;
			req = 1;
		}
		printf("Exit \n");
		req = -1;
		write(pd1[1], &req, sizeof(req));
		write(pd1[1], &PC, sizeof(PC));
		waitpid(-1, NULL, 0);
		//read from child
	}
		
	return 0;
}
