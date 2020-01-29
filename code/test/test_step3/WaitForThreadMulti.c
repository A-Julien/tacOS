/**
 * @file WaitForThread.c
 * @brief Test if Wait syscall have good comportement
 * @author Olivier Hureau
,  Hugo Feydel , Julien ALaimo
 */
#include "../../userprog/syscall.h"

void handlerSon(void * arg){
	PutString("Orde 4-\n");
	PutString("Orde 5-\n");
	ExitThread((void* ) 0);
}


void handler(void * arg){
    PutString("Orde 2-\n");
    WaitForChildExited(createUserThread((void *) handlerSon, 0));  	
    PutString("Orde 6-\n");
    ExitThread((void * ) 0x7777777);

}


int main(void){
    void * retour;
     PutString("Orde 1-\n");
    unsigned int TID = createUserThread((void *) handler, 0);
    PutString("Orde 3-\n");
   	// Wait for the son who's also waiting
   retour = WaitForChildExited(TID);
     PutString("Orde 7-\n");
    if(retour != (void * ) 0x7777777){
        PutString("Ce n'est pas le bon retour : !-\n");
        if((int) retour != 0){
            PutInt((int) retour);
            PutChar('\n');
        }

    } else {
        PutString("C'est le bon retour-\n");
    }
    // Wait all child (no more running actually)
    WaitForAllChildExited();
    PutString("Plus aucun thread a attendre-\n");
    return 0;
}