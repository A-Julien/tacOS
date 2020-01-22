/**
 * @file userthread.h
 * @brief User thread implementation
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 */

#ifndef USERTHREAD_H
#define USERTHREAD_H

#include "copyright.h"
#include "thread.h"
#include "utility.h"
#include "synchlist.h"


extern void StartUserThread(int data);


typedef struct
{
    void * f;
    void * arg;
} thread_init;




class UserThread {
	public:

		UserThread(void * ,void * arg, unsigned int tid);
		~UserThread();
		void Run();
		unsigned int getId();
		void exit(void * returnAdress);
		void * WaitForChildExited(int CID);
		// EXPERT MODE

		void WaitForAllChildExited();
		int StopChild(unsigned int CID);
		int WakeUpChild(unsigned int CID);
		int makeChildSurvive(unsigned int CID);
		void makeAllChildSurvive();
		List * getChildList();
		void DoneWithTheChildList();
		Thread *  getThread();
		bool removeChild(void * childToRemove);
		void addChildren(UserThread * UTC);
		void setParrent(UserThread * UTP);
		UserThread * getParrent();
		void * getUserThreadDataChild(unsigned int CID);
		void * getMeta();
        void setMeta(void *);
        bool isSurvivor();
        void setSurvivor(bool boolean);

	private:
		void * returnObject;
		bool WaitingForChild = false;
		Thread * thread;
		unsigned int ID;
		UserThread * parent;
		SynchList * child;
        thread_init dataFork;
        void * metaData;
        bool survivor;

};


class UserThreadData{
	public:
		UserThreadData(unsigned int ID, UserThread * UT);
		~UserThreadData();
		void setReturn(void * ret);
		void setEnd();
		void P();
		void V();
		bool isEnded();
		unsigned int getID();
		void * getReturnValue();
		UserThread * getUserThread();

	private:
		void * returnValue = this;
		bool ended = false;
		unsigned int ID;
		Semaphore * sem;
		UserThread * userthread;
		bool Dosurvive = false;
};

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


#endif // USERTHREAD_H