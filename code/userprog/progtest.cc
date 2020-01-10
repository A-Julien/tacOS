/**
 * @file progtest.cc
 * @brief Test routines for demonstrating that Nachos can load a user program and execute it.
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation
 * of liability and disclaimer of warranty provisions.
 */

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"
#include "synchconsole.h"

//----------------------------------------------------------------------
// StartProcess
//      Run a user program.  Open the executable, load it into
//      memory, and jump to it.
//----------------------------------------------------------------------

void StartProcess (char *filename)
{
    OpenFile *executable = fileSystem->Open (filename);
    AddrSpace *space;

    if (executable == NULL)
      {
	  printf ("Unable to open file %s\n", filename);
	  return;
      }
    space = new AddrSpace (executable);
    currentThread->space = space;

    delete executable;		// close file

    space->InitRegisters ();	// set the initial register values
    space->RestoreState ();	// load page table register

    machine->Run ();		// jump to the user progam
    ASSERT (FALSE);		// machine->Run never returns;
    // the address space exits
    // by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
//      Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void
ReadAvail (int arg)
{
    readAvail->V ();
}
static void
WriteDone (int arg)
{
    writeDone->V ();
}

//----------------------------------------------------------------------
// ConsoleTest
//      Test the Console by echoing characters typed at the input onto
//      the output.  Stop when the user types a ctrl+d command.
//       convert a 'c' charactere to "<c>
//   @param *in The input file  (NULL for STDIN)
//   @param *out The output file  (NULL for STDOUT)
// This function is implemented to test a the Console class.
// /!\ An error may occur : the first or the second char may be lost for when STDIN is the input /!\
//----------------------------------------------------------------------

void
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new Console (in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore ("read avail", 0);
    writeDone = new Semaphore ("write done", 0);

    for (;;){
	   readAvail->P ();	// wait for character to arrive
	   ch = console->GetChar ();

      // test if EOT/EOF
        if( in == NULL){
        
            // TTY CTRL+D equivalent is 0x4 with UNIX;
            if( ch == 0x04)
                return;
            // On windows, it seems to be -1
            if(ch == -1)
                return;    
        
        } else {
            if(ch == EOF)
                return;
        }

        if(ch == 'c'){
             console->PutChar ('<');
            writeDone->P ();
            console->PutChar ('c');
            writeDone->P ();
            console->PutChar ('>');
            writeDone->P ();
        } else {
          // echo it!
          console->PutChar (ch);
    	  writeDone->P ();	// wait for write to finish
        }
	}
}

//----------------------------------------------------------------------
// SynchConsoleTest
//      Test the SynchConsole by echoing characters typed at the input onto
//      the output.  Stop when the user types a ctrl+d command.
//       convert a 'c' charactere to "<c>
//   @param *in The input file  (NULL for STDIN)
//   @param *out The output file  (NULL for STDOUT)
// This function is implemented to test a the SynchConsole class.
// /!\ An error may occur : the first or the second char may be lost for when STDIN is the input for the first input/!\
//----------------------------------------------------------------------


void SynchConsoleTest (char *readFile, char *writeFile){
    char ch;
    SynchConsole *synchconsole = new SynchConsole(readFile, writeFile);
    while ((ch = synchconsole->SynchGetChar()) != EOF){
        if(ch == 'c'){
           synchconsole->SynchPutChar('<');
           synchconsole->SynchPutChar('c');
           synchconsole->SynchPutChar('>');

        } else {
            synchconsole->SynchPutChar(ch);
        }
    }
    
    fprintf(stderr, "Solaris: EOF detected in SynchConsole!\n");
}
