/**
 * @file Semaphore.c
 * @brief Test if Semaphore syscall have good comportement
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

void handler2(void * arg){
	PutString("T2'Take'One'Token'");
	SemaphoreP( *((int *) arg) );
	PutString("T2'Passed'");
	PutString("T2'Exit'");
	SemaphoreV( *((int *) arg) );
}

void handler1(void * arg){
	PutString("T1'Take'One'Token'");
	SemaphoreP( *((int *) arg) );
	PutString("T1'Passed'");
	PutString("T1'Exit'");
	SemaphoreV( *((int *) arg) );
}

void handler3(void * arg){
	
	SemaphoreP( *((int *) arg) );
	PutString("Ordre'4-");
	PutString("Ordre'5-");
	PutString("Ordre'6-");
	SemaphoreV( *((int *) arg) );
}

int main (void){
	// CREATE SEMAPHORE WITH TWO TOKENS
	int sem = SemaphoreCreate(2);
	// Take a token 
	SemaphoreP(sem);
	createUserThread((void *) handler1, (void *) &sem);
	createUserThread((void *) handler2, (void *) &sem);

	WaitForAllChildExited();
	unsigned int sem2 = SemaphoreCreate(1);
	SemaphoreDelete(sem);
	SemaphoreP(sem2);

	unsigned int t2 = createUserThread((void *) handler3, (void *) &sem2);
	PutString("Ordre'1-");
	PutString("Ordre'2-");
	PutString("Ordre'3-");
	SemaphoreV(sem2);
	WaitForChildExited(t2);	
	PutString("Ordre'7-");
	SemaphoreDelete(sem2);
}