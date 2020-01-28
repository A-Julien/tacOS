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
	PutInt(c1);
	res =  StopChild(7);
	if(res == 2 ){
		PutString("\nIt's not a child thread");
	}

	//StopChild(c1);
	res = StopChild(c1);
	if( res == 1){
		PutString("Child is sleeping now .. _");
	}
		if(res == 2 ){
		PutString("\nIt's not a child thread");
	} else {
		PutString("Il kiil this MF T\n");
	}

	PutString("Sleep 1\n");
	PutString("Sleep 2\n");
	PutString("Sleep 3\n");
	PutString("Sleep 4\n");
	PutString("Sleep 5\n");
	PutString("Waiking up C1_");
	
	WakeUpChild(c1);
	PutString("Sleeping C1_");

	/*PutString("CréationC2_");
	unsigned int c2 = createUserThread((void *) handler2, 0);
	 createUserThread((void *) handler2, 0);
	WaitForChildExited(c2);
	*/
	PutString("C2 finis_");
		WaitForChildExited(c1);

	Halt();
}