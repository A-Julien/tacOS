/**
 * @file UserThreadMetadata.h
 * @brief User thread implementation
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#ifndef ManagerUserMetaData_H
#define ManagerUserMetaData_H

#include "copyright.h"
#include "thread.h"
#include "utility.h"
#include "synchlist.h"




class UserThreadData{
public:
    UserThreadData(unsigned int ID, void * UT);
    ~UserThreadData();
    void setReturn(void * ret);
    void setEnd();
    void P();
    void V();
    bool isEnded();
    unsigned int getID();
    void * getReturnValue();
    void * getUserThread();

private:
    void * returnValue = this;
    bool ended = false;
    unsigned int ID;
    Semaphore * sem;
    void * userthread;
    bool Dosurvive = false;
};

#endif