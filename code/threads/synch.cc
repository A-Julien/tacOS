/// @file synch.cc                                                  
/// @brief  Data structures for synchronizing threads.             
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo           
///      Routines for synchronizing threads.  Three kinds of
///      synchronization routines are defined here: semaphores, locks 
///      and condition variables (the implementation of the last two
///      are left to the reader).
///
/// Any implementation of a synchronization routine needs some
/// primitive atomic operation.  We assume Nachos is running on
/// a uniprocessor, and thus atomicity can be provided by
/// turning off interrupts.  While interrupts are disabled, no
/// context switch can occur, and thus the current thread is guaranteed
/// to hold the CPU throughout, until interrupts are reenabled.
///
/// Because some of these routines might be called with interrupts
/// already disabled (Semaphore::V for one), instead of turning
/// on interrupts at the end of the atomic operation, we always simply
/// re-set the interrupt state back to its original value (whether
/// that be disabled or enabled).
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation 
/// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"
#include "../userprog/userthread.h"

///
/// Semaphore::Semaphore
///      Initialize a semaphore, so that it can be used for synchronization.
///
///     @param "debugName" is an arbitrary name, useful for debugging.
///     @param "initialValue" is the initial value of the semaphore.
///

Semaphore::Semaphore (const char *debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

///
/// Semaphore::Semaphore
///      De-allocate semaphore, when no longer needed.  Assume no one
///      is still waiting on the semaphore!
///

Semaphore::~Semaphore ()
{
    delete queue;
}

///
/// Semaphore::P
///      Wait until semaphore value > 0, then decrement.  Checking the
///      value and decrementing must be done atomically, so we
///      need to disable interrupts before checking the value.
///
///      Note that Thread::Sleep assumes that interrupts are disabled
///      when it is called.
///

void
Semaphore::P ()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    while (value == 0)
      {				// semaphore not available
	  queue->Append ((void *) currentThread);	// so go to sleep
	  currentThread->Sleep ();
      }
    value--;			// semaphore available, 
    // consume its value

    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts
}

///
/// Semaphore::V
///      Increment semaphore value, waking up a waiter if necessary.
///      As with P(), this operation must be atomic, so we need to disable
///      interrupts.  Scheduler::ReadyToRun() assumes that threads
///      are disabled when it is called.

void
Semaphore::V ()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel (IntOff);

    thread = (Thread *) queue->Remove ();
    if (thread != NULL)		// make thread ready, consuming the V immediately
	scheduler->ReadyToRun (thread);
    value++;
    (void) interrupt->SetLevel (oldLevel);
}

/// Dummy functions -- so we can compile our later assignments
/// Note -- without a correct implementation of Condition::Wait(),
/// the test case in the network assignment won't work!
///
/// Lock::Lock Constructeur of the Lock clss
/// \param debugName String name for debug
Lock::Lock (const char *debugName)
{
    WaitingForLock = new List();
}

///
/// Lock::~Lock Destructor of the Lock class


Lock::~Lock ()
{
   delete WaitingForLock;
}

///
/// Lock::Acquire the lock
/// If it's allready locked, put in a "waiting list" and sleep, FIFO order
void Lock::Acquire ()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
        if(currentThreadHolding == NULL){
            currentThreadHolding = currentThread;
            DEBUG('t', "Thread %d take the Lock %s.\n",  ((UserThread *) currentThread->getUserThreadAdress())->getId() , ((UserThread *) currentThread->getName()));

        } else {
            DEBUG('t', "Thread %d have been blocked by the lock %s.\n",  ((UserThread *) currentThread->getUserThreadAdress())->getId() , ((UserThread *) currentThread->getName()));
            WaitingForLock->Append((void *) currentThread);
            currentThread->setStatus(BLOCKED);
            currentThread->Yield();
        }
    (void) interrupt->SetLevel (oldLevel);
}


///
/// Lock::Release Release the lock

void Lock::Release ()
{
    Thread * next = NULL;
    IntStatus oldLevel = interrupt->SetLevel (IntOff);
        if(!WaitingForLock->IsEmpty()){
            next = (Thread *) WaitingForLock->Remove();
            scheduler->ReadyToRun(next);
        }
    currentThreadHolding = next;
    (void) interrupt->SetLevel (oldLevel);
}

Condition::Condition (const char *debugName)
{
}

Condition::~Condition ()
{
}
void
Condition::Wait (Lock * conditionLock)
{
    ASSERT (FALSE);
}

void
Condition::Signal (Lock * conditionLock)
{
}
void
Condition::Broadcast (Lock * conditionLock)
{
}
