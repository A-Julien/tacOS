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




class UserThread {
	public:

		// Create the user Thread with the function to start and the adresses of the structure object to pass to the thread.
		UserThread(VoidFunctionPtr f,void * arg);

		// Delete the user thread structure. 
		~UserThread();

		void Run();

		// Return the UserThread ID
		unsigned int getId();
		
		// Exit the thread and put the adresse of the returned objett on the return variable;
		// Only called from the thread itself.
		// If the child is in survive mode, find the new parent and asign the childrens to the parent and the parent's thread to childrens (if there is no parents, it means that the main threads is terminated.)
		// Wake the parent if is sleeping.

		void exit(void * returnAdress);

		
		// Wait for a specific child with CID id to exit. Return the return adresses of the child.
		// If returned with the adresses of the current UserThread, the id is not a child userThread ID.
		void * WaitForChildExited(int CID);
		

		// EXPERT MODE

		// Wait for all child to exited.
		// Untill there are no left child in child list.  
		void WaitForAllChildExited();

		// Stop the child who's id is CID. Return 0 if sucess, -1 if it failed, -2 if there is no child for id CID.
		// Put in the Sleep mode 
		int StopChild(int CID);

		// If the child is in sleep mod, wake it up (ie put the state to ready and add to the ready list)
		// Return 9 if sucess, -1 if the CID is'nt a Child Id, -2 if the child is'nt in sleep mode.
		int WakeUpChild(int CID);

		// Make a specific child survive after the parent died (impossible if the parrent is the main UserThread)
		int makeChildSurvive(int CID);
		// Make all the childdrens survive after the parent died (impossible if the parrent is the main UserThread)
		void makeAllChildSurvive();

		// Return the child list;
		List * getChildList();

	
		void DoneWithTheChildList();


	private: 
		// Initialized with the thread's Kernel adresse. Then if it not the same, we know it have return value. 
		void * returnObject;		
		void * args = NULL;
		// Before entering in sleep mode when waiting for child
		bool WaitingForChild = false;

		Thread * thread;
		unsigned int ID;
		// The parent's thread
		UserThread * parent;
		// List of Child
		SynchList * child;
		SynchList * SurivorID;
		VoidFunctionPtr fun;
		

		// Free the structure of UserThread, return the arg's adresse (if the void * returned is different of the thread's kernel adresse)
		void * FreeChild(int CID);
		

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
};

class ManagerUserThreadID{
	public:
		// Create the class
		ManagerUserThreadID();
		// Delete the class
		~ManagerUserThreadID();
		// Return a new ID for the thread;
		unsigned int GetNewId();
		// Add an timepassed Id to the Queue.
		void addIdFreed(unsigned int ID);
	private:
		SynchList * freeID;
		unsigned int compteur;
		Lock *lock;
};


#endif // USERTHREAD_H