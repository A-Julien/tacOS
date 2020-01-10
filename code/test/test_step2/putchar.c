/**
 * @file putchar.c
 * @brief Juste write 'abcd' to test the PutChar system function
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

void print(char c, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		PutChar(c+i);
	}
	PutChar('\n');
}
int main()
{

	print('a',4);
	Halt();
} 