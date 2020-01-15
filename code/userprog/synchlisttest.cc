#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "list.h"



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


	




	
	


	
	return 0;
	// 1 list is said empty but is not
	// 2 list is not empty but is said it is
	// 3 Append n'ajoute pas dans le bon ordre
	// 4 Prepend n'ajout pas dans le bon ordre
	// 5 On ne Get pas le bon objet
	// 6 get sur un objet avec un mauvaise index ne retourne pas null

}



