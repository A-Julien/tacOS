/* Start.s 
 *	Assembly language assist for user programs running on top of Nachos.
 *
 *	Since we don't want to pull in the entire C library, we define
 *	what we need for a user program here, namely Start and the system
 *	calls.
 */

#include "syscall.h"

        .text   
        .align  2

/* -------------------------------------------------------------
 * __start
 *	Initialize running a C program, by calling "main". 
 *
 * 	NOTE: This has to be first, so that it gets loaded at location 0.
 *	The Nachos kernel always starts a program by jumping to location 0.
 * -------------------------------------------------------------
 */

	.globl __start
	.ent	__start
__start:
	jal	main
	move	$4,$2	
	jal	Exit	 /* if we return from main, exit(0) */
	.end __start

/* -------------------------------------------------------------
 * System call stubs:
 *	Assembly language assist to make system calls to the Nachos kernel.
 *	There is one stub per system call, that places the code for the
 *	system call into register r2, and leaves the arguments to the
 *	system call alone (in other words, arg1 is in r4, arg2 is 
 *	in r5, arg3 is in r6, arg4 is in r7)
 *
 * 	The return value is in r2. This follows the standard C calling
 * 	convention on the MIPS.
 * -------------------------------------------------------------
 */

  .globl ThreadId
      .ent	ThreadId
  ThreadId:
      addiu $2,$0,SC_ThreadId
      syscall
      j	$31
      .end ThreadId

 .globl fclose
     .ent	fclose
 fclose:
     addiu $2,$0,SC_Close
     syscall
     j	$31
     .end fclose

.globl fseek
    .ent	fseek
fseek:
    addiu $2,$0,SC_Seek
    syscall
    j	$31
    .end fseek

.globl fputs
    .ent	fputs
fputs:
    addiu $2,$0,SC_Write
    syscall
    j	$31
    .end fputs

   .globl fgets
   	.ent	fgets
   fgets:
   	addiu $2,$0,SC_Read
   	syscall
   	j	$31
   	.end fgets

   .globl fopen
   	.ent	fopen
   fopen:
   	addiu $2,$0,SC_Open
   	syscall
   	j	$31
   	.end fopen

   	 .globl SemaphoreDelete
  	.ent	SemaphoreDelete
  SemaphoreDelete:
  	addiu $2,$0,SC_SemaphoreDelete
  	syscall
  	j	$31
  	.end SemaphoreDelete

  	 .globl SemaphoreV
  	.ent	SemaphoreV
  SemaphoreV:
  	addiu $2,$0,SC_SemaphoreV
  	syscall
  	j	$31
  	.end SemaphoreV


  	 .globl SemaphoreP
  	.ent	SemaphoreP
  SemaphoreP:
  	addiu $2,$0,SC_SemaphoreP
  	syscall
  	j	$31
  	.end SemaphoreP

 	 .globl SemaphoreCreate
  	.ent	SemaphoreCreate
  SemaphoreCreate:
  	addiu $2,$0,SC_SemaphoreCreate
  	syscall
  	j	$31
  	.end SemaphoreCreate


 	 .globl LockDelete
  	.ent	LockDelete
  LockDelete:
  	addiu $2,$0,SC_LockDelete
  	syscall
  	j	$31
  	.end LockDelete

 	 .globl LockRelease
  	.ent	LockRelease
  LockRelease:
  	addiu $2,$0,SC_LockRelease
  	syscall
  	j	$31
  	.end LockRelease

 	 .globl LockAcquire
  	.ent	LockAcquire
  LockAcquire:
  	addiu $2,$0,SC_LockAcquire
  	syscall
  	j	$31
  	.end LockAcquire


  .globl LockCreate
  	.ent	LockCreate
  LockCreate:
  	addiu $2,$0,SC_LockCreate
  	syscall
  	j	$31
  	.end LockCreate


  .globl makeAllChildSurvive
  	.ent	makeAllChildSurvive
  makeAllChildSurvive:
  	addiu $2,$0,SC_makeAllChildSurvive
  	syscall
  	j	$31
  	.end makeAllChildSurvive


  .globl makeChildSurvive
  	.ent	makeChildSurvive
  makeChildSurvive:
  	addiu $2,$0,SC_makeChildSurvive
  	syscall
  	j	$31
  	.end makeChildSurvive

  .globl WakeUpChild
  	.ent	WakeUpChild
  WakeUpChild:
  	addiu $2,$0,SC_WakeUpChild
  	syscall
  	j	$31
  	.end WakeUpChild

  .globl StopChild
  	.ent	StopChild
  StopChild:
  	addiu $2,$0,SC_StopChild
  	syscall
  	j	$31
  	.end StopChild



  .globl WaitForAllChildExited
  	.ent	WaitForAllChildExited
  WaitForAllChildExited:
  	addiu $2,$0,SC_WaitForAllChildExited
  	syscall
  	j	$31
  	.end WaitForAllChildExited



 .globl createUserThread
 	.ent	createUserThread
 createUserThread:
 	addiu $2,$0,SC_createUserThread
 	la  $6, ExitThread
 	syscall
 	j	$31
 	.end createUserThread

 .globl WaitForChildExited
	.ent	WaitForChildExited
WaitForChildExited:
	addiu $2,$0,SC_WaitForChildExited
	syscall
	j	$31
	.end WaitForChildExited



 .globl ExitThread
 	.ent	ExitThread
 ExitThread:
 	addiu $2,$0,SC_ExitThread
 	syscall
 	j	$31
 	.end ExitThread


   	.globl Feof
	.ent	Feof
Feof:
	addiu $2,$0,SC_Feof
	syscall
	j	$31
	.end Feof


  	.globl PutInt
	.ent	PutInt
PutInt:
	addiu $2,$0,SC_PutInt
	syscall
	j	$31
	.end PutInt

  	.globl GetInt
	.ent	GetInt
GetInt:
	addiu $2,$0,SC_GetInt
	syscall
	j	$31
	.end GetInt

  	.globl GetString
	.ent	GetString
GetString:
	addiu $2,$0,SC_GetString
	syscall
	j	$31
	.end GetString


 	.globl GetChar
	.ent	GetChar
GetChar:
	addiu $2,$0,SC_GetChar
	syscall
	j	$31
	.end GetChar



	.globl PutString
	.ent	PutString
PutString:
	addiu $2,$0,SC_PutString
	syscall
	j	$31
	.end PutString


	.globl PutChar
	.ent	PutChar
PutChar:
	addiu $2,$0,SC_PutChar
	syscall
	j	$31
	.end PutChar


	.globl Halt
	.ent	Halt
Halt:
	addiu $2,$0,SC_Halt
	syscall
	j	$31
	.end Halt

	.globl Exit
	.ent	Exit
Exit:
	addiu $2,$0,SC_Exit
	syscall
	j	$31
	.end Exit

	.globl Exec
	.ent	Exec
Exec:
	addiu $2,$0,SC_Exec
	syscall
	j	$31
	.end Exec

	.globl Join
	.ent	Join
Join:
	addiu $2,$0,SC_Join
	syscall
	j	$31
	.end Join

	.globl Create
	.ent	Create
Create:
	addiu $2,$0,SC_Create
	syscall
	j	$31
	.end Create

	.globl Open
	.ent	Open
Open:
	addiu $2,$0,SC_Open
	syscall
	j	$31
	.end Open

	.globl Read
	.ent	Read
Read:
	addiu $2,$0,SC_Read
	syscall
	j	$31
	.end Read

	.globl Write
	.ent	Write
Write:
	addiu $2,$0,SC_Write
	syscall
	j	$31
	.end Write

	.globl Close
	.ent	Close
Close:
	addiu $2,$0,SC_Close
	syscall
	j	$31
	.end Close

	.globl Fork
	.ent	Fork
Fork:
	addiu $2,$0,SC_Fork
	syscall
	j	$31
	.end Fork

	.globl Yield
	.ent	Yield
Yield:
	addiu $2,$0,SC_Yield
	syscall
	j	$31
	.end Yield

/* dummy function to keep gcc happy */
        .globl  __main
        .ent    __main
__main:
        j       $31
        .end    __main

