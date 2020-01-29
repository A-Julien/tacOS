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

int main (void){
	int res;
	PutString("CréationC1_");
	unsigned int c1 = createUserThread((void *) handler1, 0);
	// createUserThread((void *) handler1, 0);
	PutString("C1_Créé");
	// Test a stop in a non initialized thread
	res =  StopChild(7);
	if(res == 2 ){
		PutString("\nIt's not a child thread");
	}

	
	PutString("LE threadDors\n");
	// Stop the child (will no more cry !)
	StopChild(c1);

	// Try to re-stop the child
	res = StopChild(c1);
	if( res == 1){
		PutString("Child isAlready sleeping _");
	
	} if(res == 0) {
		PutString("BonRetourDeSleep\n");
	}
	// Need to be a halt because handler 1 is still alive.
	Halt();
}