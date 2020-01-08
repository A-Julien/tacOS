#include "syscall.h"


int main()
{
// STRING.size < 12
	PutString("Samourai\n");
// STRING.size = 12
	PutString("aerospatial\n");

//String.size > 12	
	PutString("Suplement SauceFromagereChef");
	PutChar('\n');
	Halt();
} 
//SORTIE = "Samourai"; "aerospatiale"; "Suplement Sa"

