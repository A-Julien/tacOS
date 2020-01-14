/**
 * @file putstring.c
 * @brief Write 3 sentences
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"


int main()
{
// STRING.size < 12
	PutString("Samourai\n");
// STRING.size = 12
	PutString("aerospatial\n");

//String.size > 12	
	PutString("Suplement SauceFromagereChef");
	PutChar('\n');
	return 0;
	//Halt();
} 


