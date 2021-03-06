/**
 * @file ManagerUserThreadID.cc
 * @brief User thread implementation
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#include "ManagerUserThreadID.h"

///
/// ManagerUserThreadID::ManagerUserThreadID() Constructor for the ManagerThreadId
/// The main thread id is 0, else it begin at 1
ManagerUserThreadID::ManagerUserThreadID(){
    freeID = new SynchList;
    compteur = 2;
    lock = new Lock ("ManagerUserThreadID lock");
    stillAlive = 0;

}

///
///ManagerUserThreadID::~ManagerUserThreadID delete the class

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
///
///  ManagerUserThreadID::GetNewId Return a ID for a new thread
/// The Id may be re-use
/// \return unsigned int TID
unsigned int ManagerUserThreadID::GetNewId(){
    int res;
    void * adressInt;
    lock->Acquire();
    if(freeID->size() > 0){
        adressInt = freeID->Remove();
        res = *((int *) adressInt);
        free(adressInt);
    } else {
        if(compteur >= 4294967295){
            return 0;
        }
        res = compteur;
        compteur++;
    }
    stillAlive++;
    lock->Release();
    return res;
}

///
/// ManagerUserThreadID::addIdFreed The TID is'nt currently used, get back to the pool.
/// \param ID unsigned int
void ManagerUserThreadID::addIdFreed(unsigned int ID){

    void * adressInt = malloc(sizeof(unsigned int));
    *((unsigned int *) adressInt) = ID;
    freeID->GetTheLock();
    freeID->Append(adressInt);
    stillAlive--;
    freeID->FreeTheLock();
}

///
/// ManagerUserThreadID::lastAlive return true if it's the las thread running (for doing an halt)
/// \return Bool a boolean
bool ManagerUserThreadID::lastAlive(){
    return (stillAlive == 0);

}