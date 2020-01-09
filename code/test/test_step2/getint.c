#include "../../userprog/syscall.h"


int main()
{
	int n;
	for(int i = 0; i < 8; i++){
		GetInt(&n);
		PutInt(n);
		PutChar('\n');
	}
	return 0;
	
} 
//SORTIE = "Samourai"; "aerospatiale"; "Suplement Sa"

