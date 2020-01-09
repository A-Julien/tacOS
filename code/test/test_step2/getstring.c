/**
 * @file getstring.c
 * @brief
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

int main(void){
	char string[30];

	GetString(string, 20);

	PutString(string);
	return 0;
	
}