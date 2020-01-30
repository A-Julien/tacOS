/// @file testImplem.cc
/// @brief  Test the list and synchlist after modification by our team
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo         
///


#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "list.h"

///
/// testList
/// @return "int" : 
/// 0 : no error
/// 1 : list is said empty but is not;
/// 2 : list is not empty but is said it is;
/// 3 : Append do not add in the right order
/// 4 : Get do not return the right objet
/// 6 : Get with a no good index don't return NULL
///

int testList (void){
	List * l = new List;
	// Test si vide fonctionne bien
	if(!l->IsEmpty()){
		return 1;
	}
	int zero = 0;
	int un = 1;
	int deux = 2;
	int trois =3;

	l->Append(&zero);
	l->Append(&un);
	l->Append(&deux);
	l->Append(&trois);

	// Test si vide fonctionne bien lorsque'elle n'est pas vide
	if(l->IsEmpty()){
		return 2;
	}


	if (*((int *) l->Remove()) != 0 ||
	 	*((int *) l->Remove()) != 1 || 
	 	*((int *) l->Remove()) != 2 || 
	 	*((int *) l->Remove()) != 3){
		return 3;
	 }
	
	

	if(!l->IsEmpty()){
		return 1;
	}

	l->Prepend(&zero);
	l->Prepend(&un);
	l->Prepend(&deux);
	l->Prepend(&trois);

	if (*((int *) l->Remove()) != 3 ||
	 	*((int *) l->Remove()) != 2 || 
	 	*((int *) l->Remove()) != 1 || 
	 	*((int *) l->Remove()) != 0){
		return 4;
	 }

	l->Append(&zero);
	l->Append(&un);
	l->Append(&deux);
	l->Append(&trois);

	if(! (  0 == *( (int *) l->get(0)) ||
			1 == *( (int *) l->get(1)) || 
			2 == *( (int *) l->get(2)) ||
			3 == *( (int *) l->get(3)) )){
		return 5;

	}
	if( !(l->get(4) == NULL || 	l->get(5) == NULL || l->get(-1) == NULL)){
		return 6;
	}
	delete l;
	return 0;


}


///
/// testUTMmono test the UserThreamManager in monothreading
/// @return "int" : 
/// 0 : no error
/// 1 : Don't give the ID in the good order.
/// 2: Don't reUse ID.
/// 3 Reuse id that allready have been associated
/// 4 Don't reuse in the good order
///

int testUTMmono(){
	unsigned int zero = managerUserThreadID->GetNewId();
	unsigned int un = managerUserThreadID->GetNewId();
	unsigned int deux = managerUserThreadID->GetNewId();
	
	if (zero != 2 ||
	 	un != 3 || 
	 	deux != 4 ){
		return 1;
	 }	

	managerUserThreadID->addIdFreed(un);
	
	unsigned int test = managerUserThreadID->GetNewId(); 

	if(test != 3){
		return 2;
	}

	unsigned int trois = managerUserThreadID->GetNewId();
	unsigned int quatre = managerUserThreadID->GetNewId();

	if(trois != 5 || quatre != 6){
		return 3;
	}

	managerUserThreadID->addIdFreed(quatre);
	managerUserThreadID->addIdFreed(trois);
	managerUserThreadID->addIdFreed(deux);
	managerUserThreadID->addIdFreed(test);
	managerUserThreadID->addIdFreed(zero);

	quatre = 2;
	trois = 2;
	deux = 2;
	un = 2;
	zero = 3;

	quatre = managerUserThreadID->GetNewId();
	trois = managerUserThreadID->GetNewId();
	deux = managerUserThreadID->GetNewId();
	un = managerUserThreadID->GetNewId();
	zero = managerUserThreadID->GetNewId();

	if (zero != 2 ||
	 	un != 3 || 
	 	deux != 4 ||
	 	trois != 5 ||
	 	quatre != 6 
	 	 ){
		return 4;
	 }	
	



	return 0;
}

int testUserThreadState(){
	return 0;

}



