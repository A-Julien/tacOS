/// @file console.cc                                         
/// @briefDataRoutines Routines to simulate a serial port to a console device.
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo    
/// console.cc 
///	Routines to simulate a serial port to a console device.
///	A console has input (a keyboard) and output (a display).
///	These are each simulated by operations on UNIX files.
///	The simulated device is asynchronous,
///	so we have to invoke the interrupt handler (after a simulated
///	delay), to signal that a byte has arrived and/or that a written
///	byte has departed.
///
///  DO NOT CHANGE -- part of the machine emulation
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation 
/// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "console.h"
#include "system.h"

// Dummy functions because C++ is weird about pointers to member functions
static void ConsoleReadPoll(int c) 
{ Console *console = (Console *)c; console->CheckCharAvail(); }
static void ConsoleWriteDone(int c)
{ Console *console = (Console *)c; console->WriteDone(); }

///
/// Console::Console
/// 	Initialize the simulation of a hardware console device.
///
///	@param "readFile" -- UNIX file simulating the keyboard (NULL -> use stdin)
///	@param "writeFile" -- UNIX file simulating the display (NULL -> use stdout)
/// @param 	"readAvail" is the interrupt handler called when a character arrives from the keyboard
/// 	@param "writeDone" is the interrupt handler called when a character has
///		been output, so that it is ok to request the next char be
///		output
///

Console::Console(char *readFile, char *writeFile, VoidFunctionPtr readAvail, 
		VoidFunctionPtr writeDone, int callArg)
{
    if (readFile == NULL)
	readFileNo = 0;					// keyboard = stdin
    else
    	readFileNo = OpenForReadWrite(readFile, TRUE);	// should be read-only
    if (writeFile == NULL)
	writeFileNo = 1;				// display = stdout
    else
    	writeFileNo = OpenForWrite(writeFile);

    // set up the stuff to emulate asynchronous interrupts
    writeHandler = writeDone;
    readHandler = readAvail;
    handlerArg = callArg;
    putBusy = FALSE;
    incoming = EOF;

    // start polling for incoming packets
    interrupt->Schedule(ConsoleReadPoll, (int)this, ConsoleTime, ConsoleReadInt);
}

///
/// Console::~Console
/// 	Clean up console emulation
///

Console::~Console()
{
    if (readFileNo != 0)
	Close(readFileNo);
    if (writeFileNo != 1)
	Close(writeFileNo);
}

///
/// Console::CheckCharAvail()
/// 	Periodically called to check if a character is available for
///	input from the simulated keyboard (eg, has it been typed?).
///
///	Only read it in if there is buffer space for it (if the previous
///	character has been grabbed out of the buffer by the Nachos kernel).
///	Invoke the "read" interrupt handler, once the character has been
///	put into the buffer.
///

void
Console::CheckCharAvail()
{
    char c;
    int n;

    // schedule the next time to poll for a packet
    interrupt->Schedule(ConsoleReadPoll, (int)this, ConsoleTime, 
			ConsoleReadInt);

    // do nothing if character is already buffered, or none to be read
    if ((incoming != EOF) || !PollFile(readFileNo))
	return;	  

    // otherwise, read character and tell user about it
    n = ReadPartial(readFileNo, &c, sizeof(char));
    incoming = (n == 1 ? c : EOF);
    stats->numConsoleCharsRead++;
    (*readHandler)(handlerArg);	
}

///
/// Console::WriteDone()
/// 	Internal routine called when it is time to invoke the interrupt
///	handler to tell the Nachos kernel that the output character has
///	completed.
///

void
Console::WriteDone()
{
    putBusy = FALSE;
    stats->numConsoleCharsWritten++;
    (*writeHandler)(handlerArg);
}

///
/// Console::GetChar()
/// 	Read a character from the input buffer, if there is any there.
///	Either return the character, or EOF if none buffered.
/// @return char The caracter available in the current FD

char
Console::GetChar()
{
   char ch = incoming;

   incoming = EOF;
   return ch;
}

///
/// Console::PutChar()
/// 	Write a character to the simulated display, schedule an interrupt
///	to occur in the future, and return.
/// @param ch : The caracter you wanted to be Put

void
Console::PutChar(char ch)
{
    ASSERT(putBusy == FALSE);
    WriteFile(writeFileNo, &ch, sizeof(char));
    putBusy = TRUE;
    interrupt->Schedule(ConsoleWriteDone, (int)this, ConsoleTime,
					ConsoleWriteInt);
}

///
/// Console::Feof
///     Initialize the simulation of a hardware console device.
///
/// @return bool : Return a bololean that indicate wether there is something more to read or not
///
bool Console::Feof(){
    if (PollFile(readFileNo))
        return false;
    return true;
}
