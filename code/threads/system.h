/// @file system.h
/// @brief  Initialization and cleanup routines
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
/// system.h
///      All global variables used in Nachos are defined here.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation
/// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#define MAX_STRING_SIZE 512
#define FLAG_ERROR_FOR_VOID ((void*) -1)

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"



extern void Initialize (int argc, char **argv);	// Initialization,
						// called before anything else
extern void Cleanup ();		// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;	// the thread holding the CPU
extern Thread *threadToBeDestroyed;	// the thread that just finished
extern Scheduler *scheduler;	// the ready list
extern Interrupt *interrupt;	// interrupt status
extern Statistics *stats;	// performance metrics
extern Timer *timer;		// the hardware alarm clock

#ifdef USER_PROGRAM
#include "machine.h"
extern Machine *machine;	// user program memory and registers
#include "synchconsole.h"
extern SynchConsole *synchConsole;
#endif

#ifdef FILESYS_NEEDED		// FILESYS or FILESYS_STUB
#include "filesys.h"
extern FileSystem *fileSystem;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice *postOffice;
#endif

#include "userthread.h"
#include "ManagerUserThreadID.h"
#include "UserThreadMetadata.h"
extern ManagerUserThreadID * managerUserThreadID;
extern ManagerUserThreadID * managerSemaphoreID;
extern ManagerUserThreadID * managerLockID;
extern char* path;

#endif // SYSTEM_H
