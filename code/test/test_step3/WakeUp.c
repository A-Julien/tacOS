/**
 * @file WakeUp.c
 * @brief Test if WakeUp syscall have good comportement
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */


#include "../../userprog/syscall.h"



void handler1(void * arg){
	int * i = (int *) arg;
	PutString("Debut handler");
	int j = 0;
	for(*i = 0; *i < 500; *i = *i +1){
		j = *i +1;
	}
	PutString("Middle");
	while(*i < 1000){
		*i = *i +1;
	}
	ExitThread((void *) j);
} 


int main (void){
	int res;
	int i = 0;
	PutInt(i);
	PutString("CréationC1_");
	unsigned int c1 = createUserThread((void *) handler1, (void *) &i);
	StopChild(c1);
	PutInt(i);
	
	PutInt(i);
	PutInt(i);
	PutInt(i);
	res =  WakeUpChild(7);
	if(res != 2 ){
		PutString("It's not a child thread");
	}
	StopChild(c1);
	res = StopChild(c1);
	if( res != 1){
		PutString("Child should be Allready WakedUp now .. _");
	}
	PutString("Attente");
	WaitForAllChildExited();
	PutString("Finis");
	Halt();
}
