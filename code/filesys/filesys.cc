/// @file  filesys.cc                                          
/// @brief Routines to manage the overall operation of the file system.
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo      
///	Implements routines to map from textual file names to files.
///
///	Each file in the file system has:
///	   A file header, stored in a sector on disk 
///		(the size of the file header data structure is arranged
///		to be precisely the size of 1 disk sector)
///	   A number of data blocks
///	   An entry in the file system directory
///
/// 	The file system consists of several data structures:
///	   A bitmap of free disk sectors (cf. bitmap.h)
///	   A directory of file names and file headers
///
///      Both the bitmap and the directory are represented as normal
///	files.  Their file headers are located in specific sectors
///	(sector 0 and sector 1), so that the file system can find them 
///	on bootup.
///
///	The file system assumes that the bitmap and directory files are
///	kept "open" continuously while Nachos is running.
///
///	For those operations (such as Create, Remove) that modify the
///	directory and/or bitmap, if the operation succeeds, the changes
///	are written immediately back to disk (the two files are kept
///	open during all this time).  If the operation fails, and we have
///	modified part of the directory and/or bitmap, we simply discard
///	the changed version, without writing it back to disk.
///
/// 	Our implementation at this point has the following restrictions:
///
///	   there is no synchronization for concurrent accesses
///	   files have a fixed size, set when the file is created
///	   files cannot be bigger than about 3KB in size
///	   there is no hierarchical directory structure, and only a limited
///	     number of files can be added to the system
///	   there is no attempt to make the system robust to failures
///	    (if Nachos exits in the middle of an operation that modifies
///	    the file system, it may corrupt the disk)
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation 
/// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "disk.h"
#include "bitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"

/// Sectors containing the file headers for the bitmap of free sectors,
/// and the directory of files.  These file headers are placed in well-known 
/// sectors, so that they can be located on boot-up.
#define FreeMapSector        0
#define DirectorySector    1

/// Initial file sizes for the bitmap and directory; until the file system
/// supports extensible files, the directory size sets the maximum number 
/// of files that can be loaded onto the disk.
#define FreeMapFileSize    (NumSectors / BitsInByte)
#define NumDirEntries        10
#define DirectoryFileSize    (sizeof(DirectoryEntry) * NumDirEntries)

///
/// FileSystem::FileSystem
/// 	Initialize the file system.  If format = TRUE, the disk has
///	nothing on it, and we need to initialize the disk to contain
///	an empty directory, and a bitmap of free sectors (with almost but
///	not all of the sectors marked as free).
///
///	If format = FALSE, we just have to open the files
///	representing the bitmap and the directory.
///
///	@param "format" -- should we initialize the disk?
///

FileSystem::FileSystem(bool format) {
    DEBUG('f', "Initializing the file system.\n");
    if (format) {
        BitMap *freeMap = new BitMap(NumSectors);
        Directory *directory = new Directory(NumDirEntries);
        FileHeader *mapHdr = new FileHeader;
        FileHeader *dirHdr = new FileHeader;

        DEBUG('f', "Formatting the file system.\n");

        // First, allocate space for FileHeaders for the directory and bitmap
        // (make sure no one else grabs these!)
        freeMap->Mark(FreeMapSector);
        freeMap->Mark(DirectorySector);

        // Second, allocate space for the data blocks containing the contents
        // of the directory and bitmap files.  There better be enough space!

        ASSERT(mapHdr->Allocate(freeMap, FreeMapFileSize));
        ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize));

        // Flush the bitmap and directory FileHeaders back to disk
        // We need to do this before we can "Open" the file, since open
        // reads the file header off of disk (and currently the disk has garbage
        // on it!).

        DEBUG('f', "Writing headers back to disk.\n");
        mapHdr->WriteBack(FreeMapSector);
        dirHdr->type = d; // because root is a directory
        dirHdr->test();
        dirHdr->WriteBack(DirectorySector);

        // OK to open the bitmap and directory files now
        // The file system operations assume these two files are left open
        // while Nachos is running.

        freeMapFile = new OpenFile(FreeMapSector);
        root_directory_file = new OpenFile(DirectorySector);
        current_directory_file = root_directory_file; //At the beginning currentDirectoryFile is the root_directory_file

        // Once we have the files "open", we can write the initial version
        // of each file back to disk.  The directory at this point is completely
        // empty; but the bitmap has been changed to reflect the fact that
        // sectors on the disk have been allocated for the file headers and
        // to hold the file data for the directory and bitmap.

        DEBUG('f', "Writing bitmap and directory back to disk.\n");
        freeMap->WriteBack(freeMapFile);     // flush changes to disk
        directory->WriteBack(root_directory_file);

        //create . and ..
        this->create_dot_and_doubledot("", DirectorySector, true);
       /* Create(".", 0, d);
        Create("..", 0, d);

        directory->FetchFrom(root_directory_file); // get update from disk

        //create file header for . and ..
        FileHeader *dot_fh = new FileHeader();
        FileHeader *double_dot_fh = new FileHeader();

        //get sectors
        int dot_sector = directory->Find(".");
        int double_dot_sector = directory->Find("..");
        //get file header of . and ..
        dot_fh->FetchFrom(dot_sector);
        double_dot_fh->FetchFrom(double_dot_sector);

        //set dot and double dot on root sector
        dot_fh->set_sector(0, DirectorySector);
        double_dot_fh->set_sector(0, DirectorySector);

        //write modifications on disk
        dot_fh->WriteBack(dot_sector);
        double_dot_fh->WriteBack(double_dot_sector);

        directory->WriteBack(root_directory_file); //commit update on root_directory_file*/

       printf("root %i", this->root_directory_file->get_sector());

        if (DebugIsEnabled('f')) {
            freeMap->Print();
            directory->Print();

            delete freeMap;
            delete directory;
            delete mapHdr;
            delete dirHdr;
        }
        this->current_sector_file = DirectorySector;
        return;
    }

    // if we are not formatting the disk, just open the files representing
    // the bitmap and directory; these are left open while Nachos is running
    freeMapFile = new OpenFile(FreeMapSector);
    root_directory_file = new OpenFile(DirectorySector);
    this->current_directory_file = root_directory_file; //At the beginning currentDirectoryFile is the root_directory_file

    this->current_sector_file = DirectorySector;
}

///
/// FileSystem::Create
/// 	Create a file in the Nachos file system (similar to UNIX create).
///	Since we can't increase the size of files dynamically, we have
///	to give Create the initial size of the file.
///
///	The steps to create a file are:
///	  Make sure the file doesn't already exist
///        Allocate a sector for the file header
/// 	  Allocate space on disk for the data blocks for the file
///	  Add the name to the directory
///	  Store the new file header on disk
///	  Flush the changes to the bitmap and the directory back to disk
///
///	@return Return TRUE if everything goes ok, otherwise, return FALSE.
///
/// 	Create fails if:
///   		file is already in directory
///	 	no free space for file header
///	 	no free entry for file in directory
///	 	no free space for data blocks for the file
///
/// 	Note that this implementation assumes there is no concurrent access
///	to the file system!
///
///	@param "name" -- name of file to be created
///	@param "initialSize" -- size of file to be created
///

bool FileSystem::Create(const char *name, int initialSize, File_type type) {
    Directory *directory;
    BitMap *freeMap;
    FileHeader *hdr;
    int sector;
    bool success;

    DEBUG('f', "Creating file %s, size %d\n", name, initialSize);

    directory = new Directory(NumDirEntries);
    directory->FetchFrom(current_directory_file);

    if (directory->Find(name) != -1)
        success = FALSE;            // file is already in directory
    else {
        freeMap = new BitMap(NumSectors);
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find();    // find a sector to hold the file header
        if (sector == -1)
            success = FALSE;        // no free block for file header
        else if (!directory->Add(name, sector))
            success = FALSE;    // no space in directory
        else {
            hdr = new FileHeader;
            if (!hdr->Allocate(freeMap, initialSize))
                success = FALSE;    // no space on disk for data
            else {
                success = TRUE;
                // everthing worked, flush all changes back to disk
                hdr->type = type;
                hdr->WriteBack(sector);
                directory->WriteBack(current_directory_file);
                freeMap->WriteBack(freeMapFile);
            }
            delete hdr;
        }
        delete freeMap;
    }
    delete directory;
    return success;
}
/// create_dot_and_doubledot
/// @brief create the "." and ".." directory in the directory
///
/// @param  the directory where . and .. will be created
void FileSystem::create_dot_and_doubledot(const char *name, int parent_sector, bool root_file){
    if(!root_file)this->change_directory(name); // go into son directory

    //create . and ..
    Create(".", 0, d);
    Create("..", 0, d);

    Directory *directory = new Directory(NumDirEntries);

    directory->FetchFrom(this->current_directory_file); // get update from disk

    //create file header for . and ..
    FileHeader *dot_fh = new FileHeader();
    FileHeader *double_dot_fh = new FileHeader();

    //get sector
    int dot_sector = directory->Find(".");
    int double_dot_sector = directory->Find("..");

    //get file header of . and ..
    dot_fh->FetchFrom(dot_sector);
    double_dot_fh->FetchFrom(double_dot_sector);

    //set dot and double dot on root sector
    dot_fh->set_sector(0, this->current_directory_file->get_sector()); //point to the current file
    double_dot_fh->set_sector(0, parent_sector);

    //write modifications on disk
    dot_fh->WriteBack(dot_sector);
    double_dot_fh->WriteBack(double_dot_sector);

    if(!root_file)this->change_directory(".."); // rollback into parent directory

    delete dot_fh;
    delete double_dot_fh;
    delete directory;
}

///
/// FileSystem::Create_new_folder
/// 	Create a new folder in the Nachos file system.
///
///	@return Return TRUE if everything goes ok, otherwise, return FALSE.
///	@param "name" -- name of file to be created
///
bool FileSystem::create_new_directory(const char *directory_name) {
    Directory *new_directory = new Directory(NumDirEntries); // init the new directory

    // create the entry in current directory if already exist, return before writing it on disk
    if(!this->Create(directory_name,DirectoryFileSize, d)) return false;

    OpenFile *open_new_directory = this->Open(directory_name); // open new directory
    new_directory->WriteBack(open_new_directory); // write new directory on disk

    //update current directory
    Directory *current_directory = new Directory(NumDirEntries); // init the new directory
    current_directory->FetchFrom(current_directory_file); //Fetch current directory
    current_directory->WriteBack(current_directory_file); //write modifications of current directory into disk

    printf("%i", this->current_directory_file->get_sector());
    this->create_dot_and_doubledot(directory_name, this->current_sector_file);

    delete open_new_directory;
    delete current_directory;
    return true;
}

/// change_directory
/// @brief change the current directory point by the filesys
/// \param name
/// \return true if success false otherwise
bool FileSystem::change_directory(const char *name){
    if(name[0] == '.' && name[1] == '\0') return true; // if the name is "." return true, nothing to do

    Directory *current_directory = new Directory(NumDirEntries);
    current_directory->FetchFrom(this->current_directory_file);
    int sector = current_directory->Find(name);

    if ( sector == -1) return false;            // directory doesn't exist

    OpenFile *openFile = this->Open(name);
    this->current_directory_file = openFile;
    this->current_sector_file = current_directory->Find(name);
    return true;
}
///
/// FileSystem::Open
/// 	Open a file for reading and writing.
///	To open a file:
///	  Find the location of the file's header, using the directory
///	  Bring the header into memory
///
///	@param "name" -- the text name of the file to be opened
///
OpenFile * FileSystem::Open(const char *name) {
    Directory *directory = new Directory(NumDirEntries);
    OpenFile *openFile = NULL;
    int sector;

    DEBUG('f', "Opening file %s\n", name);
    directory->FetchFrom(this->current_directory_file);
    sector = directory->Find(name);
    if (sector >= 0)
        openFile = new OpenFile(sector);    // name was found in directory
    delete directory;
    return openFile;                // return NULL if not found
}

///
/// FileSystem::Remove
/// 	Delete a file from the file system.  This requires:
///	    Remove it from the directory
///	    Delete the space for its header
///	    Delete the space for its data blocks
///	    Write changes to directory, bitmap back to disk
///
///	   @return Return TRUE if the file was deleted, FALSE if the file wasn't in the file system.
///
///	@param "name" -- the text name of the file to be removed
///
/// \param name
/// \return
bool FileSystem::Remove(const char *name) {
    Directory *directory;
    BitMap *freeMap;
    FileHeader *fileHdr;
    int sector;

    directory = new Directory(NumDirEntries);
    directory->FetchFrom(current_directory_file);
    sector = directory->Find(name);
    if (sector == -1) {
        delete directory;
        return FALSE;             // file not found
    }
    fileHdr = new FileHeader;
    fileHdr->FetchFrom(sector);

    freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(freeMapFile);

    fileHdr->Deallocate(freeMap);        // remove data blocks
    freeMap->Clear(sector);            // remove header block
    directory->Remove(name);

    freeMap->WriteBack(freeMapFile);        // flush to disk
    directory->WriteBack(current_directory_file);        // flush to disk
    delete fileHdr;
    delete directory;
    delete freeMap;
    return TRUE;
}

///
/// FileSystem::List
/// 	List all the files in the file system directory.
///

void FileSystem::List() {
    Directory *directory = new Directory(NumDirEntries);
    directory->FetchFrom(current_directory_file);
    directory->List();
    delete directory;
}

///
/// FileSystem::Print
/// 	Print everything about the file system:
/// 	  the contents of the bitmap
/// 	  the contents of the directory
/// 	  for each file in the directory,
/// 	      the contents of the file header
/// 	      the data in the file
///

void
FileSystem::Print() {
    FileHeader *bitHdr = new FileHeader;
    FileHeader *dirHdr = new FileHeader;
    BitMap *freeMap = new BitMap(NumSectors);
    Directory *directory = new Directory(NumDirEntries);

    printf("Bit map file header:\n");
    bitHdr->FetchFrom(FreeMapSector);
    bitHdr->Print();

    printf("Directory file header:\n");
    dirHdr->FetchFrom(DirectorySector);
    dirHdr->Print();

    freeMap->FetchFrom(freeMapFile);
    freeMap->Print();

    directory->FetchFrom(current_directory_file);
    directory->Print();

    delete bitHdr;
    delete dirHdr;
    delete freeMap;
    delete directory;
}
