#include "syscall.h"

int main(void){
	char c = GetChar();
	PutChar(c);
	return 0;
}