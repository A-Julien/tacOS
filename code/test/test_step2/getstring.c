/**
 * @file getstring.c
 * @brief Get a 20 max length string and write it.
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

int main(void){
	char string[30];

	GetString(string, 20);

	PutString(string);
	return 0;
	
}