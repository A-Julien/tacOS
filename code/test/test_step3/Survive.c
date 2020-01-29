/**
 * @file Survive.c
 * @brief Test if Survive syscall have good comportement
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

/**
 * @file newThread.c
 * @brief Test if the new Thread is correctly created
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"


void handler2(void * arg){
	PutString("ThreadPetitFils'1");
	PutString("ThreadPetitFils'2");
	PutString("ThreadPetitFils'3");
	PutString("ThreadPetitFils'4");
	PutString("ThreadPetitFils'5");
	 ExitThread((void * ) 0);
}

void handler1(void * arg){
	PutString("ThreadFils");
	unsigned int TID = createUserThread((void *) handler2, 0);
	makeChildSurvive(TID);
	ExitThread((void * ) 0);
}


int main(void){

   
    
   	WaitForChildExited(createUserThread((void *) handler1, 0));
   	PutString("Fils terminé");
    WaitForAllChildExited();
  	PutString("Tout est terminé");
    // Halt();


    return 0;
}