#include "syscall.h"



int main()
{	
	PutInt(0);
	PutChar('\n');
	PutInt(1);
	PutChar('\n');
	PutInt(12);
	PutChar('\n');
	PutInt(123);
	PutChar('\n');
	PutInt(-1);
	PutChar('\n');
	PutInt(-123);
	PutChar('\n');
	PutInt(123456789);
	PutChar('\n');
	PutInt(999999999); // Ne peux aller au dessus de ça (neuf 9)
	PutChar('\n');
	PutInt(-999999999);
	PutChar('\n');
	PutInt(-1000000000); // RETURN -1
	PutChar('\n');





	return 0;
	
} 
//SORTIE = "Samourai"; "aerospatiale"; "Suplement Sa"


