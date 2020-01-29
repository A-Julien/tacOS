/// @file userthread.cc
/// @brief  User thread implementation
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation
/// of liability and disclaimer of warranty provisions.

#include "userthread.h"
#include "../filesys/openfile.h"




///
/// UserThread::UserThread
/// \param f The function where the thread will begin
/// \param arg The adress of the object passed in parameter
/// \param tid The Thread Identifiant
UserThread::UserThread(void * f, void * arg, unsigned int tid, int exitPC){
    char * buffer = (char *) malloc(50*sizeof(char));
    sprintf(buffer, "Thread NO : %d", tid);
    free(buffer);
    this->thread = new Thread(buffer);
    dataFork.arg = arg;
    dataFork.f = f;
    dataFork.exit = exitPC;
    this->ID = tid; // MODIFY WHEN ID ALLOCATOR;
    this->child = new SynchList();
    this->TableOfOpenfile = (int*) malloc(sizeof(int) * MAX_OPEN_FILE);
    survivor = false;
}

int* UserThread::getTableOfOpenfile(){
    return this->TableOfOpenfile;
}

///
/// UserThread::~UserThread Delete the UserThread class
UserThread::~UserThread(){

    //free(TableOfOpenfile);

    delete child;

}

///
/// UserThread::getThread Get the Thread of the UserThread
/// \return Thread *
Thread * UserThread::getThread(){
	return thread;
}
///
///  UserThread::getChildList Get the child list and put a lock on it
/// when done, don't forget to use DoneWithTheChildList();
/// \return List *
///
List * UserThread::getChildList(){
	child->GetTheLock();
	return child->getList();
}

///
/// UserThread::DoneWithTheChildList trash the lock Locked from getChildList
void UserThread::DoneWithTheChildList(){
	child->FreeTheLock();
}



///
/// UserThread::Run Do the fork and place the thread into the ready list
void UserThread::Run() {
    // Be careful, i casted there in int but in the documentation it's void*
    thread->Fork(StartUserThread, (int) &dataFork);
}



///
/// UserThread::getId() get The Thread Id
/// \return unsigned int ID
unsigned int UserThread::getId(){
	return ID;
}

///
///  UserThread::exit Finis the thread. Put information on the current UserThreadData if it had a parent.
/// \param returnAdress void * : The adress of the object you want to return

void UserThread::exit(void * returnAdress){
	UserThreadData * state;
	if(parent == NULL){
		// CHECK IF GOOD IN DOCUMENTATION
		thread->Finish();
		return;
	}
	// Get the UserThreadData corresponding
	List * l = parent->getChildList();
	for(unsigned int i = 0; i < l->size(); i++){
		state = (UserThreadData *) l->get(i);
		if( state->getID() == ID){
			break;
		}
	}
	parent->DoneWithTheChildList();

	// Check if the UserDataThread is the UsertThread's one
	if(state->getID() != ID){
	    // TRAITER L'ERREUR
	}
	// Put in the userthread usefull information for the parrent
	// Ant put a token on the semaphore to eventually wake the parent
    state->setReturn(returnAdress);
    state->setEnd();
    state->V();

	thread->Finish();
}

///
/// UserThread::WaitForChildExited Current thraid will wait that his child end
/// \param CID The child TID
/// \return void * the adress of the object the child wanted to return.
/// Return -1 if it's not a child's TiD
void * UserThread::WaitForChildExited(int CID){
	void * res;
	UserThreadData * state =  (UserThreadData *) getUserThreadDataChild(CID);
	if(state == NULL ){
		return (void *) -1;
	}
	state->P();

	res = state->getReturnValue();
    // return bool
	removeChild((void *) state);

    	// Remove le userThread
	delete ( (UserThread *) state->getUserThread());
	delete state;

	return res; 
}

// EXPERT MODE :

///
/// UserThread::WaitForAllChildExited Wait for all the child exited
void UserThread::WaitForAllChildExited(){

    while(!child->IsEmpty()){
        UserThreadData * WaitedChild = (UserThreadData *) child->get(0);
        WaitForChildExited(WaitedChild->getID());


    }
}





///  UserThread::makeChildSurvive Pass a child in suvivor mode
/// \param CID Child id
/// \return 1 if the TID is not a child ID, 0 otherwise
int UserThread::makeChildSurvive(unsigned int CID){
    UserThreadData * state = (UserThreadData *) getUserThreadDataChild(CID);
    if(state == NULL){
        return 1;
    }
    ((UserThread *) state->getUserThread())->setSurvivor(true);
    DEBUG('t', "Thread %d is now in survivor mode\n ", state->getID());
	return 0;
}
///
/// UserThread::makeAllChildSurvive Put all the child of the current thread in survivor mode
void UserThread::makeAllChildSurvive(){
    List * l = getChildList();
    for(unsigned int i = 0; i < l->size(); i++){
        UserThread * enfant = (UserThread *) ((UserThreadData *) l->get(i))->getUserThread();
        enfant->setSurvivor(true);
    }
    DoneWithTheChildList();

}


///
/// UserThread::removeChild Remove the child of the current child list
/// \param childToRemove
/// \return
bool UserThread::removeChild(void * childToRemove){
    bool res;
    List* childList = getChildList();
    res = childList->removeElement(childToRemove);
    DoneWithTheChildList();
    return res;
}

///
/// UserThread::addChildren Add a thread to the child list
/// \param UTC UserThread *
void UserThread::addChildren(UserThread * UTC){
    UserThreadData * childData = new UserThreadData(UTC->getId(), UTC);
    UTC->setMeta(childData);
    child->Append((void *) childData);
    ASSERT(childData->getID() == UTC->getId());
}

///
/// UserThread::setParrent Add a parrent to the current thread
/// \param UTP UserThread * Parrent
void UserThread::setParrent(UserThread * UTP){
    parent = UTP;
}

///
/// UserThread::getUserThreadDataChild Get the Meta Data structure of the child with TID CID
/// \param CID
/// \return A void * or NULL if the not a child

void * UserThread::getUserThreadDataChild(unsigned int CID){
   List * l = getChildList();
   for(unsigned int i = 0; i < l->size(); i++){
       UserThreadData * UTD = (UserThreadData *) l->get(i);
       if (UTD->getID() == CID){
           DoneWithTheChildList();
           return (void * ) UTD;
       }
   }
    DoneWithTheChildList();
    return NULL;
}
///
/// UserThread::getMeta return the adress of the metaStructure
/// \return void *
void * UserThread::getMeta(){
    return metaData;
}
///
/// UserThread::setMeta Set a metaData for the current thread
/// \param meta
void UserThread::setMeta(void * meta){
    metaData = meta;
}

///
/// UserThread::isSurvivor Test if the current thread is in mode survivor
/// \return
bool UserThread::isSurvivor(){
    return survivor;
}

///
///  UserThread::setSurvivor Enable or disable the survivor mode
/// \param boolean True to enabe, false to disable

void  UserThread::setSurvivor(bool boolean){
    survivor = boolean;
}

///
///  UserThread::getParrent Get the UserThread * parrent
/// \return UserThread *
UserThread * UserThread::getParrent(){
    return parent;
}
