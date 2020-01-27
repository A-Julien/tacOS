/**
 * @file synchconsole.h
 * @brief Synchrone Data structures to simulate the behavior of a terminal I/O device.
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 * Synchconsole do the same work as console but synchronously.
 */

#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"

class SynchConsole {
	public:
		SynchConsole(char *readFile, char *writeFile);
		// initialize the hardware console device
		~SynchConsole(); // clean up console emulation
		void SynchPutChar(const char ch); // Unix putchar(3S)
		int SynchGetChar(); // Unix getchar(3S)
		void SynchPutString(const char *s); // Unix puts(3S)
		void SynchGetString(char *s, int n); // Unix fgets(3S)
		bool Feof(); // Unix Feof
        int fopen(const char* filename, int tid);
        int fgets(char* into, int FileDescriptor, int numBytes, int tid);

        private:

    Console *console;
};
#endif // SYNCHCONSOLE_H
