/// @file addrspace.h
/// @brief Data structures to keep track of executing user programs
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
///
///      Data structures to keep track of executing user programs
///      (address spaces).
///
///      For now, we don't keep any information about address spaces.
///      The user level CPU state is saved and restored in the thread
///      executing the user program (see thread.h).
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation
/// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"

#define UserStackSize		1024	// increase this as necessary!

class AddrSpace
{
  public:
    /// Create an address space,
    AddrSpace (OpenFile * executable);
    // initializing it with the program
    // stored in the file "executable"

    /// De-allocate an address space
    ~AddrSpace ();

    /// Initialize user-level CPU registers,
    void InitRegisters ();
    // before jumping to user code

    /// Save/restore address space-specific
    void SaveState ();

    /// info on a context switch
    void RestoreState ();

  private:
    /// Assume linear page table translation
      TranslationEntry * pageTable;
    // for now!
    /// Number of pages in the virtual
    unsigned int numPages;
    // address space
};

#endif // ADDRSPACE_H
