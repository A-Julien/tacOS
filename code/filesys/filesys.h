/// @file  filesys.h
/// @brief Data structures to represent the Nachos file system.
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
///
///	A file system is a set of files stored on disk, organized
///	into directories.  Operations on the file system have to
///	do with "naming" -- creating, opening, and deleting files,
///	given a textual file name.  Operations on an individual
///	"open" file (read, write, close) are to be found in the OpenFileTable
///	class (openfile.h).
///
///	We define two separate implementations of the file system.
///	The "STUB" version just re-defines the Nachos file system
///	operations as operations on the native UNIX file system on the machine
///	running the Nachos simulation.  This is provided in case the
///	multiprogramming and virtual memory assignments (which make use
///	of the file system) are done before the file system assignment.
///
///	The other version is a "real" file system, built on top of
///	a disk simulator.  The disk is simulated using the native UNIX
///	file system (in a file named "DISK").
///
///	In the "real" implementation, there are two key data structures used
///	in the file system.  There is a single "root" directory, listing
///	all of the files in the file system; unlike UNIX, the baseline
///	system does not provide a hierarchical directory structure.
///	In addition, there is a bitmap for allocating
///	disk sectors.  Both the root directory and the bitmap are themselves
///	stored as files in the Nachos file system -- this causes an interesting
///	bootstrap problem when the simulated disk is initialized.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation
/// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"
#include "filehdr.h"

#define MAX_OPEN_FILE 20
#define ROOT_DIRECTORY_FILE 0
#define CURRENT_DIRECTORY_FILE 1

#ifdef FILESYS_STUB        // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available


class FileSystem {
public:
    FileSystem(bool format) {}

    bool Create(const char *name, int initialSize) {
    int fileDescriptor = OpenForWrite(name);

    if (fileDescriptor == -1) return FALSE;
    Close(fileDescriptor);
    return TRUE;
    }

    OpenFileTable* Open(char *name) {
    int fileDescriptor = OpenForReadWrite(name, FALSE);

    if (fileDescriptor == -1) return NULL;
    return new OpenFileTable(fileDescriptor);
    }

    bool Remove(char *name) { return Unlink(name) == 0; }
};

#else // FILESYS

typedef struct path_parse {
    char** pathSplit;
    int size;
} path_parse_t;

typedef struct file_table {
    unsigned int tid;
    char *path;
    OpenFile** OpenFileTable;
    struct file_table *next;
} file_table_t;

typedef struct global_file_table {
    OpenFile *openFile;
    struct global_file_table *next;
} global_file_table_t;

class FileSystem {
public:
    FileSystem(bool format);        // Initialize the file system.
    // Must be called *after* "synchDisk"
    // has been initialized.
    // If "format", there is nothing on
    // the disk, so initialize the directory
    // and the bitmap of free blocks.

    bool Create(const char *name, int initialSize, File_type type = f);

    bool MkDir(const char *directory_name, unsigned int tid = 0); // Create a folder

    bool CdDir(const char *directory_name, unsigned int tid = 0); // Change the current folder

    bool RmDir(const char *directory_name, unsigned int tid = 0);// Remove a folder

    path_parse_t* CdFromPathName(const char *path_name, unsigned int tid = 0, int truncate = 0);

    OpenFile* OpenFromPathName(const char* path_name, unsigned int tid = 0);

    bool MkdirFromPathName(const char* path_name, unsigned int tid);

    OpenFile *Open(const char *name, unsigned int tid = 0);    // Open a file (UNIX open)

    bool Remove(const char *name, unsigned int tid = 0);    // Delete a file (UNIX unlink)

    void List();            // List all the files in the file system

    void Print();            // List all the files and their contents

    void registerOpenFileTable(int* table, unsigned int tid);

    bool unregisterOpenFileTable(unsigned int tid);

    int UserOpen(const char *name, unsigned int tid);

    int UserRead(int fileDescriptor, char *into, int numBytes, unsigned int tid);

    int UserWrite(int fileDescriptor, char *from, int numBytes, unsigned int tid);

    void UserSetSeek(int fileDescriptor, int position, unsigned int tid);

    int UserCloseFile(int fileDescriptor, int* threadTableFileDescriptor, unsigned int tid);

    bool MkdirFromPathName(const char* path_name, unsigned int tid);
        private:
    bool add_to_openFile_table(OpenFile *openFile, OpenFile **table = NULL);

    OpenFile *get_open_file_by_sector(int sector);

    int close_file(OpenFile* openFile, OpenFile** threadFileTable, unsigned int tid);

    int removeFiletoGlobalTable(OpenFile* openFile);

    OpenFile** get_thread_file_table(unsigned int tid);

    void addFiletoGlobalTable(OpenFile *openFile);

    void init_ThreadsFilesTable();

    int getFileDescriptor(OpenFile* openFile,unsigned int tid);

    void initFileDesciptortable(int* table);

    void initOpenFileTable(OpenFile** table);

    path_parse_t* parse(char *path_name);

    global_file_table_t *GlobalOpenFileTable;
    OpenFile *freeMapFile;          // Bit map of free disk blocks, represented as a file
    file_table_t *ThreadsFilesTable;

};

#endif // FILESYS

#endif // FS_H
