/**
 * @file Lock.c
 * @brief Test if Lock syscall have good comportement
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */


#include "../../userprog/syscall.h"

void handler1(void * arg){
	LockAcquire(*((int *)arg));
	PutString("Ordre4-");
	PutString("Ordre5-");
	PutString("Ordre6-");
	LockRelease(*((int *)arg));
}

void handler2(void * arg){
	LockAcquire(*((int *)arg));
	PutString("Ordre7-");
	PutString("Ordre8-");
	PutString("Ordre9-");
	LockRelease(*((int *)arg));
}





int main (void){
	// CREATE SEMAPHORE WITH TWO TOKENS
	int lock = LockCreate();
	// Acquire the lock
	LockAcquire(lock);
	createUserThread((void *) handler1, (void *) &lock);
	createUserThread((void *) handler2, (void *) &lock);
	PutString("Ordre1-");
	PutString("Ordre2-");
	PutString("Ordre3-");
	LockRelease(lock);
	WaitForAllChildExited();
	LockDelete(lock);
	

}