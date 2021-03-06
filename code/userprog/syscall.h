/**
 * @file syscal.h
 * @brief This file is included by user programs and by the Nachos kernel.
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 *
 *  Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation
 * of liability and disclaimer of warranty provisions.
 */

/* syscalls.h

 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */
#define SC_Halt		0
#define SC_Exit		1
#define SC_Exec		2
#define SC_Join		3
#define SC_Create	4
#define SC_Open		5
#define SC_Read		6
#define SC_Write	7
#define SC_Close	8
#define SC_Fork		9
#define SC_Yield	10
#define SC_PutChar  11
#define SC_PutString 12
#define SC_GetChar 13
#define SC_GetString 14
#define SC_GetInt 15
#define SC_PutInt 16
#define SC_Feof 17
#define SC_Seek 18

// ID Reservé pour Olivier et les threads de 20 à 40

#define SC_createUserThread 20
#define SC_WaitForChildExited 21
#define SC_ExitThread 22
#define SC_WaitForAllChildExited 23
#define SC_makeAllChildSurvive 24
#define SC_makeChildSurvive 25
#define SC_WakeUpChild 26
#define SC_StopChild 27
#define SC_ThreadEndedWithoutExit 28
#define SC_ThreadId 29

#define SC_LockCreate 30
#define SC_LockAcquire 31
#define SC_LockRelease 32
#define SC_LockDelete 33
#define SC_SemaphoreCreate 34
#define SC_SemaphoreP 35
#define SC_SemaphoreV 36
#define SC_SemaphoreDelete 37


#ifdef IN_USER_MODE

// LB: This part is read only on compiling the test/*.c files.
// It is *not* read on compiling test/start.S


/* The system call interface.  These are the operations the Nachos
 * kernel needs to support, to be able to run user programs.
 *
 * Each of these is invoked by a user program by simply calling the 
 * procedure; an assembly language stub stuffs the system call code
 * into a register, and traps to the kernel.  The kernel procedures
 * are then invoked in the Nachos kernel, after appropriate error checking, 
 * from the system call entry point in exception.cc.
 */

/* Stop Nachos, and print out performance stats */
void Halt () __attribute__((noreturn));


/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit (int status) __attribute__((noreturn));

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;

/* Run the executable, stored in the Nachos file "name", and return the 
 * address space identifier
 */
SpaceId Exec (char *name);

/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */
int Join (SpaceId id);


/* File system operations: Create, Open, Read, Write, Close
 * These functions are patterned after UNIX -- files represent
 * both files *and* hardware I/O devices.
 *
 * If this assignment is done before doing the file system assignment,
 * note that the Nachos file system has a stub implementation, which
 * will work for the purposes of testing out these routines.
 */

/* A unique identifier for an open Nachos file. */
typedef int OpenFileId;

/* when an address space starts up, it has two open files, representing 
 * keyboard input and display output (in UNIX terms, stdin and stdout).
 * Read and Write can be used directly on these, without first opening
 * the console device.
 */

#define ConsoleInput	0
#define ConsoleOutput	1

/* Create a Nachos file, with "name" */
void Create (char *name);

/** Open the Nachos file "name", and return an "OpenFileId" that can
 *
 * be used to read and write to the file.
 */
OpenFileId Open (char *name);

/** Write "size" bytes from "buffer" to the open file. */
void Write (char *buffer, int size, OpenFileId id);

/**
 * Read "size" bytes from the open file into "buffer".
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough 
 * characters to read, return whatever is available (for I/O devices, 
 * you should always wait until you can return at least one character).
 */
int Read (char *buffer, int size, OpenFileId id);

/**
 * Close the file, we're done reading and writing to it.
 * */
void Close (OpenFileId id);



/**
 * User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program. 
 */

/**
 * Fork a thread to run a procedure ("func") in the *same* address space
 * as the current thread.
 */
void Fork (void (*func) ());

/**
 * Yield the CPU to another runnable thread, whether in this address space
 * or not. 
 */
void Yield ();


//
// PutChar
// Write a char in the current SynchConsole
// @param c : the char
//

void PutChar( char c);


//
// PutChar
// Write a String in the current SynchConsole
// Only one syscall is made there
// @param s : The string
//

void PutString(char * string);


//
// GetChar
// Get the char written in the current SynchConsole
// @return char
//

int GetChar();

//
// GetString
// Get the a String written in the current SynchConsole
// Only one syscall is made there
// @param string : The adresse where the string will be writted
// @param taille : The max size of the string.
//

void GetString(char * string, int taille);

//
// GetInt
// Get the int written in the current SynchConsole
// @param *n, the adress where the in will be writted.
//

void GetInt(int * n);


//
// PutInt
// Write a int in the current SynchConsole
// @param n : The int
//



void PutInt(int n);

//
// Feof
// Know if there is char to be read in the SynchConsole
// @return 1 if feof 0 instead
//
//

int Feof();
unsigned int  createUserThread(void * f,void * arg);
void* WaitForChildExited(unsigned int CID);
void ExitThread(void * object);
// Expert mode
void WaitForAllChildExited();
int StopChild(int CID);
int WakeUpChild(int CID);
int makeChildSurvive(int CID);
void makeAllChildSurvive();
unsigned int ThreadId();

int fopen(const char* filename);
int fgets(int fileDescriptor, char* into, int numBytes);
int fputs(int fileDescriptor, char* from, int numBytes);
void fseek(int fileDescriptor, int position);
int fclose(int fileDescriptor);
int LockCreate();
void LockAcquire(unsigned int ID);
void LockRelease(unsigned int ID);
int LockDelete(unsigned int ID);
int SemaphoreCreate(int nbTokens);
void SemaphoreP(unsigned int ID);
void SemaphoreV(unsigned int ID);
int SemaphoreDelete(unsigned int ID);



#endif // IN_USER_MODE

#endif /* SYSCALL_H */
