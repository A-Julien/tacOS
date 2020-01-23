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
#include "ManagerUserThreadID.h"
#include "UserThreadMetadata.h"

#include "openfile.h"


extern void StartUserThread(int data);


typedef struct
{
    void * f;
    void * arg;
    int exit;
} thread_init;


class UserThread {
public:

		UserThread(void * ,void * arg, unsigned int tid, int exitPC);
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
		OpenFile** getTableOfOpenfile();

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
		OpenFile **TableOfOpenfile;

};

#endif // USERTHREAD_H