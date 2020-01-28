/**
 * @file Semaphore.c
 * @brief Test if Semaphore syscall have good comportement
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"
int main (void){
	PutString("Testing");
	Halt();
	return 0;
}