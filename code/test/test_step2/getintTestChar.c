/**
 * @file getint.c
 * @brief Test if the exception is correctly raised
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
	GetInt(&n);
	PutInt(n);
	PutChar('\n');

	return 0;
	
} 
//SORTIE = "Samourai"; "aerospatiale"; "Suplement Sa"

