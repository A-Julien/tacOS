/**
 * @file newThread.c
 * @brief Test if the new Thread is correctly created
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

void handler(void * arg){
    PutString("Je suis l'enfant ;)\n");
    PutString("Et je ne me suis pas encore fais avorté\n");
    ExitThread((void * ) 0x7777777);
   return;
}




int main(void){
    int i;
    void * retour;
    unsigned int TID = createUserThread((void *) handler, 0);
    PutString("Naissance de l'enfant\n");
    for(i = 0; i < 10000; i++){
        i++;
    }
    PutString("#Effondrement\n");
   retour = WaitForChildExited(TID);
    PutString("L'enfant est mort\n");
    if(retour != (void * ) 0x7777777){
        PutString("Ce n'est pas le bon retour : !\n");
        if((int) retour != 0){
            PutInt((int) retour);
            PutChar('\n');
        }

    } else {
        PutString("C'est le bon retour\n");
    }
    WaitForAllChildExited();
    PutString("Plus aucun thread à attendre\n");
    Halt();

    return 0;
}