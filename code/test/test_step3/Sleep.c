/**
 * @file Sleep.c
 * @brief Test if Sleep syscall have good comportement
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */


#include "../../userprog/syscall.h"

void handler1(){
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	PutString("\nJe suis debout !_\n");
	
} 

void handler2(){	
	PutString("C2-Naissance_");
	//PutString("")
}

int main (void){
	int res;
	PutString("CréationC1_");
	unsigned int c1 = createUserThread((void *) handler1, 0);
	// createUserThread((void *) handler1, 0);
	PutString("C1_Créé");
	
	res =  StopChild(7);
	if(res == 2 ){
		PutString("\nIt's not a child thread");
	}

	
	PutString("LE threadDors\n");
		StopChild(c1);

	res = StopChild(c1);
	if( res == 1){
		PutString("Child isAlready sleeping _");
	
	} if(res == 0) {
		PutString("BonRetourDeSleep\n");
	}

	Halt();
}