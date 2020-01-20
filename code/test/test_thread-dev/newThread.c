/**
 * @file newThread.c
 * @brief Test if the new Thread is correctly created
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

void handler(void * arg){
    PutString("Je suis l'enfant ;)");
    ExitThread(0);
}




int main(void){
    unsigned int TID = createUserThread((void *) handler, 0);
    PutString("Naissance de l'enfant");
   WaitForChildExited(TID);
    int i = 0;
    i++;
    i++;
    i++;
    //Halt();

    return 0;
}