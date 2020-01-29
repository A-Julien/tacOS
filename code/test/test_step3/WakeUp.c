/**
 * @file WakeUp.c
 * @brief Test if WakeUpChild syscall have good comportement
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */


#include "../../userprog/syscall.h"

void handler1(){
	PutString("\nAwake");
	PutString("\nBaillement");
	PutString("WAKED");

	

	
} 


int main (void){
	int res;


	res = WakeUpChild(7);
	if( res != 2){
		PutString("should not be a child\n");
	}

	unsigned int c1 = createUserThread((void *) handler1, 0);
	
	res = WakeUpChild(c1);
	
	 if(res == 0) {
		PutString("should be waked up\n");
	} else if(res == 2){
		PutString("should be a child\n");

	}
	PutString("Endore l'enfant");
	StopChild(c1);
	PutString("DODO-");
	PutString("l'enfantDO-");
	PutString("Reveil sonne-");
	WakeUpChild(c1);
	res = WakeUpChild(c1);
	if( res == 2){
		PutString("should be a child");
	
	} if(res == 0) {
		PutString("should not re wake\n");
	}
   	WaitForChildExited(c1);
   	
}