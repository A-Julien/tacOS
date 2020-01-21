/// @file list.h                                            
/// @brief  User thread implementation
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo         
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation 
/// of liability and disclaimer of warranty provisions.

#include "userthread.h"
#include "../filesys/openfile.h"


UserThreadData::UserThreadData(unsigned int tid, UserThread *UT) {
    this->ID = tid;
    this->userthread = UT;
    this->sem = new Semaphore("UserThreadSemaphore", 0);
    this->TableOfOpenfile = (OpenFile**) malloc(sizeof(OpenFile) * MAX_OPEN_FILE);
}

UserThreadData::~UserThreadData() {
    delete this->sem;
    delete this->TableOfOpenfile;
}

void UserThreadData::setReturn(void *ret) {
    returnValue = ret;
}

void UserThreadData::setEnd() {
    ended = true;
}

void UserThreadData::P() {
    sem->P();
}

void UserThreadData::V() {
    sem->V();
}

bool UserThreadData::isEnded() {
    return ended;
}

unsigned int UserThreadData::getID() {
    return ID;
}

void * UserThreadData::getReturnValue() {
    return returnValue;
}

UserThread * UserThreadData::getUserThread() {
    return userthread;
}


ManagerUserThreadID::ManagerUserThreadID() {
    freeID = new SynchList;
    compteur = 1;
    lock = new Lock("ManagerUserThreadID lock");

}

ManagerUserThreadID::~ManagerUserThreadID() {
    void *adressToFree;

    List *l = freeID->getList();
    freeID->GetTheLock();
    for (unsigned int i = 0; i < freeID->size(); i++) {
        adressToFree = l->Remove();
        free(adressToFree);
    }
    freeID->FreeTheLock();

    delete lock;
    delete freeID;
}

unsigned int ManagerUserThreadID::GetNewId() {
    int res;
    void *adressInt;
    lock->Acquire();
    if (freeID->size() > 0) {
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

void ManagerUserThreadID::addIdFreed(unsigned int ID) {

    void *adressInt = malloc(sizeof(unsigned int));
    *((unsigned int *) adressInt) = ID;
    freeID->Append(adressInt);


}

OpenFile** UserThreadData::getTableOfOpenfile(){
    return this->TableOfOpenfile;
}

UserThread::UserThread(void *f, void *arg, unsigned int tid) {
    char *buffer = (char *) malloc(50 * sizeof(char));
    sprintf(buffer, "Thread NO : %d", tid);
    thread = new Thread(buffer);
    dataFork.arg = arg;
    dataFork.f = f;
    ID = tid; // MODIFY WHEN ID ALLOCATOR;
    child = new SynchList();
}

Thread * UserThread::getThread() {
    return thread;
}

List *UserThread::getChildList() {
    child->GetTheLock();
    return child->getList();
}

void UserThread::DoneWithTheChildList() {
    child->FreeTheLock();
}


void UserThread::Run() {
    // Be careful, i casted there in int but in the documentation it's void*

    thread->Fork(StartUserThread, (int) &dataFork);


}

UserThread::~UserThread() {
    delete child;
    delete SurivorID;
}

unsigned int UserThread::getId() {
    return ID;
}

void UserThread::exit(void *returnAdress) {
    UserThreadData *state;
    if (parent == NULL) {
        // CHECK IF GOOD IN DOCUMENTATION
        thread->Finish();
        return;
    }
    List *l = parent->getChildList();
    for (unsigned int i = 0; i < l->size(); i++) {
        state = (UserThreadData *) l->get(i);
        if (state->getID() == ID) {
            break;
        }
    }


    parent->DoneWithTheChildList();
    if (state->getID() != ID) {
        // TRAITER L'ERREUR
    }
    state->setReturn(returnAdress);
    state->setEnd();
    state->V();
    thread->Finish();


}

void * UserThread::WaitForChildExited(int CID) {
    void *res;
    UserThreadData *state;
    List *l = getChildList();
    for (unsigned int i = 0; i < l->size(); i++) {
        state = (UserThreadData *) l->get(i);
        if (state->getID() == ID) {
            break;
        }
    }
    DoneWithTheChildList();

    // Ce n'était pas le bon enfant
    // Raise exception ?
    if (state == NULL)  return (void *) this;

    state->P();
    res = state->getReturnValue();
    // return bool
    removeChild((void *) state);


    // Remove le userThread
    delete (state->getUserThread());
    delete state;

    return res;
}

// EXPERT MODE :

void UserThread::WaitForAllChildExited() {
    while (!child->IsEmpty()) {
        UserThreadData *WaitedChild = (UserThreadData *) child->get(0);
        WaitForChildExited(WaitedChild->getID());
    }
}

int UserThread::StopChild(int CID) {
    return 0;
}

int UserThread::WakeUpChild(int CID) {
    return 0;
}

int UserThread::makeChildSurvive(int CID) {
    return 0;
}

void UserThread::makeAllChildSurvive() {

}

void * UserThread::FreeChild(int CID) {
    return NULL;

}


bool UserThread::removeChild(void *childToRemove) {
    bool res;
    List *childList = getChildList();
    res = childList->removeElement(childToRemove);
    DoneWithTheChildList();
    return res;
}

void UserThread::addChildren(UserThread *UTC) {
    UserThreadData *childData = new UserThreadData(UTC->getId(), UTC);
    child->Append((void *) childData);
}

void UserThread::setParrent(UserThread *UTP) {
    parent = UTP;
}


void *UserThread::getUserThreadDataChild(unsigned int CID) {
    List *l = getChildList();
    for (unsigned int i = 0; i < l->size(); i++) {
        UserThreadData *UTD = (UserThreadData *) l->get(i);
        if (UTD->getID() == CID) {
            DoneWithTheChildList();
            return (void *) UTD;
        }
    }
    DoneWithTheChildList();
    return NULL;
}
