/**
 * @file ManagerUserThreadID.h
 * @brief User thread implementation
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#ifndef ManagerUserThreadID_H
#define ManagerUserThreadID_H

#include "copyright.h"
#include "thread.h"
#include "utility.h"
#include "synchlist.h"

class ManagerUserThreadID{
public:
    ManagerUserThreadID();
    ~ManagerUserThreadID();
    unsigned int GetNewId();
    void addIdFreed(unsigned int ID);
private:
    SynchList * freeID;
    unsigned int compteur;
    Lock *lock;
};

#endif