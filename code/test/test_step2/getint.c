#include "../../userprog/syscall.h"


int main()
{
	int n;
	//while(1){
		GetInt(&n);
		PutInt(n);
		PutChar('\n');
	//}
	return 0;
	
} 
//SORTIE = "Samourai"; "aerospatiale"; "Suplement Sa"

