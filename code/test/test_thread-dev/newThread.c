/**
 * @file newThread.c
 * @brief Test if the new Thread is correctly created
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

void handler(void * arg){
    PutString("Je suis l'enfant ;)\n");
    PutString("Et je ne me suis pas encore fais avorté\n");
    ExitThread(0);
}




int main(void){
    int i;
    unsigned int TID = createUserThread((void *) handler, 0);
    PutString("Naissance de l'enfant\n");
    for(i = 0; i < 10000; i++){
        i++;
    }
    PutString("#Effondrement\n");
   WaitForChildExited(TID);
    PutString("L'enfant est mort\n");
    Halt();

    return 0;
}