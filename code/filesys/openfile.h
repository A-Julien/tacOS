/// @file  openfile.h
/// @brief 	Data structures for opening, closing, reading and writing
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
///	Data structures for opening, closing, reading and writing to
///	individual files.  The operations supported are similar to
///	the UNIX ones -- type 'man open' to the UNIX prompt.
///
///	There are two implementations.  One is a "STUB" that directly
///	turns the file operations into the underlying UNIX operations.
///	(cf. comment in filesys.h).
///
///	The other is the "real" implementation, that turns these
///	operations into read and write disk sector requests.
///	In this baseline implementation of the file system, we don't
///	worry about concurrent accesses to the file system
///	by different threads -- this is part of the assignment.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation
/// of liability and disclaimer of warranty provisions.

#ifndef OPENFILE_H
#define OPENFILE_H

#include "copyright.h"
#include "utility.h"

#ifdef FILESYS_STUB            // Temporarily implement calls to
// Nachos file system as calls to UNIX!
// See definitions listed under #else
class OpenFile {
public:
OpenFile(int f) { file = f; currentOffset = 0; }	// open the file
~OpenFile() { Close(file); }			// close the file

int ReadAt(char *into, int numBytes, int position) {
Lseek(file, position, 0);
return ReadPartial(file, into, numBytes);
}
int WriteAt(const char *from, int numBytes, int position) {
Lseek(file, position, 0);
WriteFile(file, from, numBytes);
return numBytes;
}
int Read(char *into, int numBytes) {
int numRead = ReadAt(into, numBytes, currentOffset);
currentOffset += numRead;
return numRead;
}
int Write(const char *from, int numBytes) {
int numWritten = WriteAt(from, numBytes, currentOffset);
currentOffset += numWritten;
return numWritten;
}

int Length() { Lseek(file, 0, 2); return Tell(file); }

private:
int file;
int currentOffset;
};

#else // FILESYS

class FileHeader;

///
/// tuple
/// Structure that allow multi-seek on one Openfile
///
typedef struct tuple {
    int seekPosition;
    unsigned int tid;
    struct tuple *next;
} tuple_t;

class OpenFile {
public:
    OpenFile(int sector);        // Open a file whose header is located
    // at "sector" on the disk
    ~OpenFile();            // Close the file

    void Seek(int position, unsigned int tid = 0);        // Set the position from which to
    // start reading/writing -- UNIX lseek

    int Read(char *into, int numBytes, unsigned int tid = 0); // Read/write bytes from the file,
    // starting at the implicit position.
    // Return the # actually read/written,
    // and increment position in file.
    int Write(const char *from, int numBytes, unsigned int tid = 0);

    int ReadAt(char *into, int numBytes, int position);

    // Read/write bytes from the file,
    // bypassing the implicit position.
    int WriteAt(const char *from, int numBytes, int position);

    int Length();            // Return the number of bytes in the
    // file (this interface is simpler
    // than the UNIX idiom -- lseek to
    // end of file, tell, lseek back

    int get_sector();

    void add_seek(unsigned int tid);

    bool remove_seek(unsigned int tid);

    bool isOpenByOthers();


private:
    void set_seek_position(unsigned int tid, int seekPosition);

    int get_seek_position(unsigned int tid);


    FileHeader *hdr;            // Header for this file
    tuple_t *seek_tid_list;

};

#endif // FILESYS

#endif // OPENFILE_H
