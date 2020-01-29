/**
 * @file newThread.c
 * @brief Test if the new Thread is correctly created
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "../../userprog/syscall.h"

void handler(void * arg){
   int i = 1;
   int k = 0;
   for(int j = 0; j < 1000; j++){
        i = i;
    }
    if(k != 0){
        PutString("Fils pas content :(");
    }
    PutString("Adresse argument \n");
    PutInt((int) ((int *) arg));
    i = 777;
    if(*((int *) arg) != 666){
        PutString("Je n'ai pas eu la bonne Value...");
    }
   ExitThread((void * ) i);
}


int main(void){

    int i = 666;
    int j = 0;
    int k = 0;
    int l = 0;
    unsigned int TID = createUserThread((void *) handler, (void *) &i);
    int m = 0;
    int n = 0;

    for(k =0 ; k < 1000; k++){
        k = k;
    }
    int * retour = ( (int * ) WaitForChildExited(TID));
    int adresseInt = (int) retour;
    //PutString("Value retour = :");
    //PutInt(*retour);

    if(adresseInt == (int) &i || adresseInt == (int) &j ||adresseInt == (int) &k ||adresseInt == (int) &l ||adresseInt == (int) &m ||adresseInt == (int) &n ){
        PutString("SP n'est pas bon\n");
          if(i != 666){
                PutString("C'est la variable I du père qui a était modifié\n");
            }
            if(j != 0){
                PutString("C'est la variable j du père qui a était modifié");
            }
            if(k != 0){
                PutString("C'est la variable k du père qui a était modifié");
            }
            if(l != 0){
                PutString("C'est la variable l du père qui a était modifié");
            }
            if(m != 0){
                PutString("C'est la variable m du père qui a était modifié");
            }
            if(n != 0){
                PutString("C'est la variable n du père qui a était modifié");
            }
    }
    PutString("\n&retour");
    PutInt((int) &i);
    PutString("\n&i = ");
    PutInt((int) &i);
    PutString("\n&j = ");
    PutInt((int) &j);
    PutString("\n&lk= ");
    PutInt((int) &k);
    PutString("\n&l = ");
    PutInt((int) &l);
    PutString("\n&m = ");
    PutInt((int) &m);
    PutString("\n&n = ");
    PutInt((int) &n);
    
  
    // Halt();


    return 0;
}