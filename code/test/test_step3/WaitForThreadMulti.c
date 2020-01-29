/**
 * @file WaitForThread.c
 * @brief Test if Wait syscall have good comportement
 * @author Olivier Hureau
,  Hugo Feydel , Julien ALaimo
 */
#include "../../userprog/syscall.h"

void handlerSon(){
	PutString("Orde 5-\n");
	PutString("Orde 7-\n");
	ExitThread((void* ) 0);
}


void handler(void * arg){
    PutString("Orde 2-\n");
    unsigned int TID = createUserThread((void *) handlerSon, 0);
    PutString("Orde 4-\n");
   	int i = (int) WaitForChildExited(TID);
   	PutInt(i);
    PutString("Orde 5-\n");
    ExitThread((void * ) 0x7777777);

}


int main(void){
    PutString("Debug");
    Halt();
    void * retour;
     PutString("Orde 1-\n");
    unsigned int TID = createUserThread((void *) handler, 0);
    PutString("Orde 3-\n");
   	
   retour = WaitForChildExited(TID);
     PutString("Orde 6-\n");
    if(retour != (void * ) 0x7777777){
        PutString("Ce n'est pas le bon retour : !-\n");
        if((int) retour != 0){
            PutInt((int) retour);
            PutChar('\n');
        }

    } else {
        PutString("C'est le bon retour-\n");
    }
    WaitForAllChildExited();
    PutString("Plus aucun thread a attendre-\n");
    // Halt();
    Halt();
    return 0;
}