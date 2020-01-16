/// @file list.h                                            
/// @brief  User thread implementation
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo         
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation 
/// of liability and disclaimer of warranty provisions.

#include "userthread.h"

UserThreadData::UserThreadData(unsigned int tid, UserThread * UT){
	ID = tid;
	userthread = UT;
	sem = new Semaphore("UserThreadSemaphore", 0);
}

UserThreadData::~UserThreadData(){
	delete sem;
}

void
UserThreadData::setReturn(void * ret){
	returnValue = ret;
}

void
UserThreadData::setEnd(){
	ended = true;
}

void
UserThreadData::P(){
	sem->P();
}

void
UserThreadData::V(){
	sem->V();
}

bool UserThreadData::isEnded(){
	return ended;
}

unsigned int 
UserThreadData::getID(){
	return ID;
}

void * 
UserThreadData::getReturnValue(){
	return returnValue;
}

UserThread * 
UserThreadData::getUserThread(){
	return userthread;
}




ManagerUserThreadID::ManagerUserThreadID(){
	freeID = new SynchList;
	compteur = 0;
	lock = new Lock ("ManagerUserThreadID lock");
   
}

ManagerUserThreadID::~ManagerUserThreadID(){
	void * adressToFree;
	
	List * l = freeID->getList();
	freeID->GetTheLock();
	for(unsigned int i = 0; i < freeID->size(); i++){
		adressToFree = l->Remove();
		free(adressToFree);
	} 
	freeID->FreeTheLock();

	delete lock;
	delete freeID;
}

unsigned int 
ManagerUserThreadID::GetNewId(){
	int res;
	void * adressInt;
	lock->Acquire();
	if(freeID->size() > 0){
		adressInt = freeID->Remove();
		res = *((int *) adressInt);
		free(adressInt);
	} else {
		res = compteur;
		compteur++;
	}
	lock->Release();
	return res;
}

void 
ManagerUserThreadID::addIdFreed(unsigned int ID){
	
	void * adressInt = malloc(sizeof(unsigned int));
	*((unsigned int *) adressInt) = ID;
	freeID->Append(adressInt);

	

}

UserThread::UserThread(VoidFunctionPtr f,void * arg){
	thread = new Thread("User's thread");
	args = arg;
	fun = f;
	ID  = 1; // MODIFY WHEN ID ALLOCATOR;
}

List * UserThread::getChildList(){
	child->GetTheLock();
	return child->getList();
}

void UserThread::DoneWithTheChildList(){
	child->FreeTheLock();
}

void
UserThread::Run(){
	// Be careful, i casted there in int but in the documentation it's void*
	thread->Fork(fun, (int) args);
}

UserThread::~UserThread(){
	delete child;
	delete SurivorID;
}

unsigned int 
UserThread::getId(){
	return ID;
}

void 
UserThread::exit(void * returnAdress){
	UserThreadData * state;
	if(parent == NULL){
		// CHECK IF GOOD IN DOCUMENTATION
		thread->Finish();
		return;
	}
	List * l = parent->getChildList();
	for(unsigned int i = 0; i < l->size(); i++){
		state = (UserThreadData *) l->get(i);
		if( state->getID() == ID){
			state->setReturn(returnAdress);
			state->setEnd();
			break;
		}
	}
	

	parent->DoneWithTheChildList();
	thread->Finish();


}

void * 
UserThread::WaitForChildExited(int CID){
	void * res;
	UserThreadData * state;
	List * l = parent->getChildList();
	for(unsigned int i = 0; i < l->size(); i++){
		state = (UserThreadData *) l->get(i);
		if( state->getID() == ID){
			break;
		}
	}
	parent->DoneWithTheChildList();
	if(state == NULL){
		// Ce n'était pas le bon enfant
		// Raise exception ?
		return (void *) this;
	} 
	state->P();
	res = state->getReturnValue();
	// TODO REMOVE DE LA LIST DES CHILDS
	//addIdFreed() ? 

	// Remove le userThread
	delete (state->getUserThread());
	delete state;
	
	return res; 
}

// EXPERT MODE :

void 
UserThread::WaitForAllChildExited(){

}

int 
UserThread::StopChild(int CID){
	return 0;
}

int
UserThread::WakeUpChild(int CID){
	return 0;
}

int 
UserThread::makeChildSurvive(int CID){
	return 0;
}

void 
UserThread::makeAllChildSurvive(){

}

void * 
UserThread::FreeChild(int CID){
	return NULL;

}
