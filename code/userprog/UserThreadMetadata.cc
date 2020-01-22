/// @file UserThreadMetadata.cc
/// @brief  User thread implementation
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation
/// of liability and disclaimer of warranty provisions.

#include "UserThreadMetadata.h"

///
/// UserThreadData::UserThreadData Constructor for UserThreadData
/// \param tid Unsigned int id of the thread
/// \param UT Adress of the UserThread
UserThreadData::UserThreadData(unsigned int tid, void * UT){
    ID = tid;
    userthread = UT;
    sem = new Semaphore("UserThreadSemaphore", 0);
}


///
/// UserThreadData::~UserThreadData Destructor for UserThreadData


UserThreadData::~UserThreadData(){
    delete sem;
}
///
/// UserThreadData::setReturn Set a return adress for the thread
/// \param ret

void UserThreadData::setReturn(void * ret){
    returnValue = ret;
}

///
/// UserThreadData::setEnd Indicate that the thread is over
///

void UserThreadData::setEnd(){
    ended = true;
}

///
/// UserThreadData::P Take a token

void UserThreadData::P(){
    sem->P();
}

///
/// UserThreadData::V put a token

void
UserThreadData::V(){
    sem->V();
}

///
/// UserThreadData::isEnded Boolean function information
/// \return true if the thread is over, false otherwise
bool UserThreadData::isEnded(){
    return ended;
}
/// UserThreadData::getID get the thread ID
/// \return unsigned int TID
unsigned int UserThreadData::getID(){
    return ID;
}

///
///  UserThreadData::getReturnValue get the adress of the returned object of the thread
/// \return void *
void * UserThreadData::getReturnValue(){
    return returnValue;
}

///
///  UserThreadData::getUserThread() Get the userThread pointer
/// \return void *
void * UserThreadData::getUserThread(){
    return userthread;
}