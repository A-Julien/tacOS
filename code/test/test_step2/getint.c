/**
 * @file getint.c
 * @brief get an int and write it in the console
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

///
/// 
/// For the needs of CI test, do a loop of 10 read/write int.
/// 
///


int main()
{
	int n;
	for(int i = 0; i < 11; i++){
		GetInt(&n);
		PutInt(n);
		PutChar('\n');
	}
    Halt();
	return 0;
	
} 
//SORTIE = "Samourai"; "aerospatiale"; "Suplement Sa"

