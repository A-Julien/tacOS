/**
 * @file newThread.c
 * @brief Test if the new Thread is correctly created
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

void handler(void * arg){
//    PutString("Deuxieme thread");
    ExitThread(0);
}

void handler2(void){
    PutString("Deuxieme thread");
    ExitThread(0);
}



int main(void){
    unsigned int TID = createUserThread((void *) handler, 0);
    PutInt((int) TID);
    PutInt((int) TID);
    Halt();

    return 0;
}