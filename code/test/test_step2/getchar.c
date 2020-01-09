/**
 * @file getchar.c
 * @brief
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */
#include "../../userprog/syscall.h"

int main(void){
	char c = GetChar();
	PutChar(c);
	return 0;
}