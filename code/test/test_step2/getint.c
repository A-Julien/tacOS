/**
 * @file getint.c
 * @brief
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"


int main()
{
	int n;
	for(int i = 0; i < 11; i++){
		GetInt(&n);
		PutInt(n);
		PutChar('\n');
	}
	return 0;
	
} 
//SORTIE = "Samourai"; "aerospatiale"; "Suplement Sa"

