/**
 * @file WaitForThread.c
 * @brief Test if Wait syscall have good comportement
 * @author Olivier Hureau
,  Hugo Feydel , Julien ALaimo
 */
#include "../../userprog/syscall.h"



void handler(void * arg){
    char string[14];
    char string2[15];
    if(arg == ((void *)0) ){
        GetString(string, 14);
        string[0]='@';
        PutString(string);
    } else {

    GetString(string2, 15);
     string2[0]='#';
    PutString(string2);
    }

}


int main(void){

    
    createUserThread((void *) handler, 0);
    createUserThread((void *) handler, (void * ) 1);
    WaitForAllChildExited();
}