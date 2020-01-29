/**
 * @file newThread.c
 * @brief Test if the new Thread is correctly created
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

void handler(void * arg){
    PutString("\nalive\n");
   // ExitThread(0);
}

int main(void){
	// Test if the programm ended well with no return NOR exitThread();
    PutString("Création");
    unsigned int TID = createUserThread((void *) handler, (void *) 0x99);
    WaitForChildExited(TID);

    PutString("Killed");



   // return 0;
}


