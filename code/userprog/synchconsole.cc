/// @file console.cc                                         
/// @briefDataRoutines Routines to simulate a serial port to a console device.
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo    
/// console.cc 
///	Routines to simulate a serial port to a console device.
///	A console has input (a keyboard) and output (a display).
///	These are each simulated by operations on UNIX files.
///	Instead of console, Synchconsole, it simulate device synchonously,


#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "openfile.h"
#include "synch.h"
#include "filesys.h"
#include "../threads/synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *writeAvail;



///
/// ReadAvail
/// give token to the semaphore for reading
/// Handler gived to Console 
///

static void ReadAvail(int arg) { readAvail->V(); }
///
/// WriteDone
/// give token to the semaphore for writing
/// Handler gived to Console 
///
static void WriteDone(int arg) { writeDone->V(); }

///
/// SynchConsole::SynchConsole
/// 	Initiate semaphore and Console class.
///
///	@param "readFile" : -- UNIX file simulating the keyboard (NULL -> use stdin)
///	@param "writeFile":  -- UNIX file simulating the display (NULL -> use stdout)
///
/// 
///	
///
SynchConsole::SynchConsole(char *readFile, char *writeFile){
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
    writeAvail = new Semaphore ("can write", 1);
	console =  new Console (readFile, writeFile, ReadAvail, WriteDone, 0);

}

///
/// SynchConsole::~SynchConsole
/// 	Clean up the SynchConsole
///
SynchConsole::~SynchConsole(){
	delete console;
	delete writeDone;
	delete readAvail;
    delete writeAvail;
}

///
/// SynchConsole::SynchPutChar
/// Write a char in the output stream
/// @param ch : the caracter that will be written
void SynchConsole::SynchPutChar(const char ch){
	// Do the work there for the synch for user.
	// PutChar will automaticly wait for a character to be written.
	// ie - the console had to call WriteDone Handler to continue the execution.

    writeAvail->P();
    console->PutChar(ch);
    writeDone->P();
    writeAvail->V();
}

///
/// SynchConsole::SynchGetChar
/// Wait for a char to be available and return is int value
/// @return int : The int value of the char readed
//
int SynchConsole::SynchGetChar(){
	// Do the work there for the synch for user.
	// GetChar will automaticly wait for a charactere to be available.
	// ie - the console had to call ReadHavailbe Handler to continue the execution.
	int res;


	readAvail->P();
	 res = console->GetChar();

	 return res;
}
///
/// SynchConsole::SynchPutString
/// Write a String in the output stream. This is more efficent than multiple PutChar beacause
// Only one syscall is made.
/// @param s : the char * that will by written (Care of the '\o' end)
//
void SynchConsole::SynchPutString(const char s[]){
	// While there is no end of String ('\0'), put the current char
	int i = 0;

	while(s[i] != '\0'){
        writeAvail->P();
        console->PutChar(s[i]);
        writeDone->P();
        writeAvail->V();
		i++;
	}


}

///
/// SynchConsole::SynchPutString
/// Get a String from the stream. This is more efficent than multiple GetChar beacause
// Only one syscall is made.
/// @param  *s : is the adress where the string will be writed
/// @param n : The max size of the string. The string is ended if one char is wether '\n' or EOF
//
void SynchConsole::SynchGetString(char *s, int n){
	// For n time, get a character
	// If c is '\n' or EOF or '\0' we assume it the end of the string.

	for(int i = 0; i < n; i++){
		readAvail->P();
        char c = console->GetChar();
		if(c == '\n' || c == EOF || c == '\0'){
			s[i] = '\0'; return;
		}
		s[i] = c ;
	}
	s[n] = '\0';

}

///
/// SynchConsole::Feof
/// Test wether the output stream have available charactere to read.
/// @return Bool, true if the file doesn't have any charactere to be readed
///
bool SynchConsole::Feof(){
	// Call the Feof fonction of Console
	return console->Feof();
}

int SynchConsole::fopen(const char* filename, int tid) {
    OpenFile * openFile = fileSystem->Open(filename, tid);
    return (int)((void *)openFile);
}

