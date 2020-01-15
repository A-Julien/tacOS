/// @file list.h                                            
/// @brief  User thread implementation
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo         
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation 
/// of liability and disclaimer of warranty provisions.

#include "userthread.h"

UserThread::UserThread(void * f (void *),void * arg){
	thread = new Thread("User's thread");
	args = arg;
}

void
UserThread::Run(){

}

UserThread::~UserThread(){
}

unsigned int 
UserThread::getId(){
	return 0;
}

void 
UserThread::exit(void * returnAdress){

}

void * 
UserThread::WaitForChildExited(int CID){
	return NULL;
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
