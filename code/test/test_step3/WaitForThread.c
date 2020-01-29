/**
 * @file WaitForThread.c
 * @brief Test if Wait syscall have good comportement
 * @author Olivier Hureau
,  Hugo Feydel , Julien ALaimo
 */
#include "../../userprog/syscall.h"

void handlerSon(){
	PutString("Orde 3-\n");
	PutString("Orde 4-\n");
	ExitThread((void* ) 0);
}


void handler(void * arg){
    PutString("Orde 2-\n");
    
    PutString("Orde 4-\n");
    if(WaitForChildExited(0) != (void *) -1){
        PutString("Wait Had to Return bad Child in son");
    }
    PutString("Orde 5-\n");
    ExitThread((void * ) 0x7777777);

}


int main(void){

    void * retour;
    PutString("WaitBadChild");
    if(WaitForChildExited(0) != (void *) -1){
        PutString("Wait Had to Return bad Child");
    }
     PutString("\nOrde 1-\n");
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