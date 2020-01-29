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
	makeChildSurvive( createUserThread((void *) handler2, 0));
	//WaitForAllChildExited();
}


int main(void){

   
    unsigned int TID = createUserThread((void *) handler1, 0);
   	WaitForChildExited(TID);
   	PutString("Fils terminé");
    WaitForAllChildExited();
  	PutString("Tout est terminé");
    // Halt();


    return 0;
}