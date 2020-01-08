#include "syscall.h"

int main(void){
	char string[12];
	GetString(string, 10);
	PutString(string);
	return 0;
	
}