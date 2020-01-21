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
#define FreeMapSector      0
#define DirectorySector    1

/// Initial file sizes for the bitmap and directory; until the file system
/// supports extensible files, the directory size sets the maximum number 
/// of files that can be loaded onto the disk.
#define FreeMapFileSize    (NumSectors / BitsInByte)
#define NumDirEntries       10
#define DirectoryFileSize   (sizeof(DirectoryEntry) * NumDirEntries)


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

    //init table
    this->init_ThreadsFilesTable();

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

        // Adding the special directories "." and ".."
        directory->Add(".", DirectorySector);
        directory->Add("..", DirectorySector); //special case for root : ".." is itself

        // OK to open the bitmap and directory files now
        // The file system operations assume these two files are left open
        // while Nachos is running.

        freeMapFile = new OpenFile(FreeMapSector);
        this->ThreadsFilesTable->thread_table[ROOT_DIRECTORY_FILE] = new OpenFile(DirectorySector);
        this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE] = this->ThreadsFilesTable->thread_table[ROOT_DIRECTORY_FILE]; //At the beginning currentDirectoryFile is the root_directory_file

        // Once we have the files "open", we can write the initial version
        // of each file back to disk.  The directory at this point is completely
        // empty; but the bitmap has been changed to reflect the fact that
        // sectors on the disk have been allocated for the file headers and
        // to hold the file data for the directory and bitmap.

        DEBUG('f', "Writing bitmap and directory back to disk.\n");
        freeMap->WriteBack(freeMapFile);     // flush changes to disk
        directory->WriteBack(this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE]);


        if (DebugIsEnabled('f')) {
            freeMap->Print();
            directory->Print();

            delete freeMap;
            delete directory;
            delete mapHdr;
            delete dirHdr;
        }
        return;
    }

    // if we are not formatting the disk, just open the files representing
    // the bitmap and directory; these are left open while Nachos is running
    freeMapFile = new OpenFile(FreeMapSector);
    this->ThreadsFilesTable->thread_table[ROOT_DIRECTORY_FILE] = new OpenFile(DirectorySector);
    this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE] = this->ThreadsFilesTable->thread_table[ROOT_DIRECTORY_FILE]; //At the beginning currentDirectoryFile is the root_directory_file

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
    directory->FetchFrom(this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE]);

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
                // everything worked, flush all changes back to disk
                hdr->type = type;
                hdr->WriteBack(sector);
                directory->WriteBack(this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE]);
                freeMap->WriteBack(freeMapFile);
            }
            delete hdr;
        }
        delete freeMap;
    }
    delete directory;
    return success;
}

///
/// FileSystem::CdDir
/// 	Come in folder given.
///
///	@return Return TRUE if everything goes ok, otherwise, return FALSE.
///	@param "name" -- name of folder to come in
///
bool FileSystem::CdDir(const char *directory_name) {
    OpenFile *new_dir_f;

    if ((new_dir_f = Open(directory_name)) == NULL) return FALSE; //folder doesn't exist

    delete this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE];

    if (directory_name[0] == '/' and directory_name[1] == '\0') { // folder "/" is root
        this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE] = this->ThreadsFilesTable->thread_table[ROOT_DIRECTORY_FILE];
        return TRUE;
    }

    this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE] = new_dir_f;
    return TRUE;
}

///
/// FileSystem::MkDir
/// 	Create a new folder in the Nachos file system.
///
///	@return Return TRUE if everything goes ok, otherwise, return FALSE.
///	@param "name" -- name of folder to be created
///
bool FileSystem::MkDir(const char *directory_name) {
    BitMap *freeMap;
    Directory *current_dir;
    Directory *new_dir;
    OpenFile *new_dir_f;
    FileHeader *new_dir_hdr;
    int new_sector;

    //Get current directory
    current_dir = new Directory(NumDirEntries);
    current_dir->FetchFrom(this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE]);

    if (current_dir->Find(directory_name) != -1) return FALSE;  // folder already exists

    // Get free sector from bitmap
    freeMap = new BitMap(NumSectors);
    freeMap->FetchFrom(freeMapFile);
    new_sector = freeMap->Find();                               // find a sector to hold the file header

    if (new_sector == -1) return FALSE;                         // no free block for file header

    if (!current_dir->Add(directory_name, new_sector)) return FALSE;        // no space in directory

    new_dir_hdr = new FileHeader;

    if (!new_dir_hdr->Allocate(freeMap, DirectoryFileSize)) return FALSE;     // no space on disk for data

    // Adding the special folders "." and ".."
    new_dir = new Directory(NumDirEntries);
    new_dir->Add(".", new_sector);
    new_dir->Add("..", current_dir->Find("."));

    // everything worked, flush all changes back to disk
    new_dir_hdr->type = d;
    new_dir_hdr->WriteBack(new_sector);
    freeMap->WriteBack(freeMapFile);
    new_dir_f = new OpenFile(new_sector);
    new_dir->WriteBack(new_dir_f);
    current_dir->WriteBack(this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE]);

    delete freeMap;
    delete new_dir_hdr;
    delete new_dir_f;
    delete new_dir;
    delete current_dir;

    return TRUE;
}

///
/// FileSystem::RmDir
/// 	Remove the folder given.
///
///	@return Return TRUE if everything goes ok, otherwise, return FALSE.
///	@param "name" -- name of folder to be removed
///
bool FileSystem::RmDir(const char *directory_name) {
    if(strcmp(directory_name, ".") || strcmp(directory_name, "..") || strcmp(directory_name, "/")) return false;
    Directory *current_dir, *to_be_rm_dir;
    BitMap *freemap;
    FileHeader *file_hdr;
    OpenFile *to_be_rm_file;
    int sector;

    // Get the current directory
    current_dir = new Directory(NumDirEntries);
    current_dir->FetchFrom(this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE]);

    // Find sector
    sector = current_dir->Find(directory_name);

    if (sector == -1) return FALSE;

    // Get the header
    file_hdr = new FileHeader;
    file_hdr->FetchFrom(sector);

    // Get the directory to remove
    to_be_rm_dir = new Directory(NumDirEntries);
    to_be_rm_file = this->get_open_file_by_sector(sector);
    if (!to_be_rm_file) {
        to_be_rm_file = new OpenFile(sector);
    }
    to_be_rm_dir->FetchFrom(to_be_rm_file);

    if (!to_be_rm_dir->isEmpty()) { //directory not empty
        delete to_be_rm_dir;
        delete file_hdr;
        delete to_be_rm_file;
        return FALSE;
    }

    // Get BitMap
    freemap = new BitMap(NumSectors);
    freemap->FetchFrom(freeMapFile);

    // Let's remove
    file_hdr->Deallocate(freemap);        // remove data blocks
    freemap->Clear(sector);            // remove header block
    current_dir->Remove(directory_name);

    // Write changes
    freemap->WriteBack(freeMapFile);        // flush to disk
    current_dir->WriteBack(this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE]);        // flush to disk

    //remove to_be_rm_file to the openfile_table
    //this->remove_open_file(to_be_rm_file);

    delete to_be_rm_dir;
    delete file_hdr;
    delete to_be_rm_file;
    delete current_dir;
    delete freemap;
    return TRUE;
}
/// FileSystem::get_open_file_by_sector
/// search if the openfile stored at "sector" are already open by kernel or thread
/// \param sector the file to get
/// \return the OpenFile* or NULL if the file are not open
OpenFile* FileSystem::get_open_file_by_sector(int sector) {
    global_file_table_t *list = this->GlobalOpenFileTable;
    while (list != NULL) {
        if(list->openFile->get_sector() == sector) return list->openFile;
        list = list->next;
    }
    return NULL;
}

/// Remove an OpenFile from the table
/// Be carefull, this function does not close the openfile
/// \param openFile the openfile
/// \return true fi succeed, false of openfile is not found in the table
bool FileSystem::remove_open_file(OpenFile* openFile) {
    for (int i = 2; i < MAX_OPEN_FILE; i++) {
        if (this->ThreadsFilesTable->thread_table[i] == openFile) {
            this->ThreadsFilesTable->thread_table[i] = NULL;
            return true;
        }
    }
    return false;
}

void FileSystem::addFiletoGlobalTable(OpenFile* openFile){

   if(this->GlobalOpenFileTable == NULL){ //if head is NULL
       this->GlobalOpenFileTable = (global_file_table_t*) malloc(sizeof(global_file_table_t));
       this->GlobalOpenFileTable->openFile = openFile;
       return;
   }

    global_file_table_t* gfileTable = this->GlobalOpenFileTable; //get head

    // go at the end of linked list
    while(gfileTable->next != NULL) gfileTable = gfileTable->next;

    gfileTable->next = (global_file_table_t*) malloc(sizeof(global_file_table_t));
    gfileTable->openFile = openFile;
}

///
/// FileSystem::Open
/// 	Open a file for reading and writing.
///	To open a file:
///	  Find the location of the file's header, using the directory
///	  Bring the header into memory
///
///	@param "name" -- the text name of the file to be opened
/// \return openfile, or NULL if error
OpenFile* FileSystem::Open(const char *name, unsigned int tid) {
    Directory *directory = new Directory(NumDirEntries);
    OpenFile *openFile = NULL;
    int sector;

    DEBUG('f', "Opening file %s\n", name);
    directory->FetchFrom(this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE]);
    sector = directory->Find(name);
    if (sector >= 0) { // name was found in directory
        openFile = this->get_open_file_by_sector(sector);

        bool needDelete = false;
        if (openFile == NULL) {
            openFile = new OpenFile(sector);
            needDelete = true;
        }

        openFile->add_seek(tid);

        OpenFile **thread_table = this->get_thread_file_table(tid);

        this->addFiletoGlobalTable(openFile);

        if (!this->add_to_openFile_table(openFile, thread_table)) {
            if(needDelete)delete openFile;
            delete directory;
            return NULL;
        }
    }
    delete directory;
    return openFile;                // return NULL if not found
}

OpenFile **FileSystem::get_thread_file_table(unsigned int tid) {
    file_table_t *list = this->ThreadsFilesTable;

    while (list != NULL) {
        if (list->tid == tid) return list->thread_table;
        list = list->next;
    };
    return NULL;
}

/// Add an openfile to the this->ThreadsFilesTable->thread_table
/// \param openFile the openfile to add
/// \return true if succeed, false if MAX_OPEN_FILE reached
bool FileSystem::add_to_openFile_table(OpenFile* openFile, OpenFile **table) {
    //if(table == NULL) table = this->ThreadsFilesTable->thread_table;
    for (int i = 0; i < MAX_OPEN_FILE; i++) {
        if (table[i] == NULL) {
            table[i] = openFile;
            return true;
        }
    }
    return false;
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
bool FileSystem::Remove(const char *name) {
    Directory *directory;
    BitMap *freeMap;
    FileHeader *fileHdr;
    int sector;

    directory = new Directory(NumDirEntries);
    directory->FetchFrom(this->ThreadsFilesTable->thread_table[ROOT_DIRECTORY_FILE]);
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
    directory->WriteBack(this->ThreadsFilesTable->thread_table[ROOT_DIRECTORY_FILE]);        // flush to disk
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
    directory->FetchFrom(this->ThreadsFilesTable->thread_table[CURRENT_DIRECTORY_FILE]);
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
void FileSystem::Print() {
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

    directory->FetchFrom(this->ThreadsFilesTable->thread_table[ROOT_DIRECTORY_FILE]);
    directory->Print();

    delete bitHdr;
    delete dirHdr;
    delete freeMap;
    delete directory;
}

/// Init a openfile table
/// \param table
void FileSystem::init_table(OpenFile **table) {
    for (int i = 0; i < MAX_OPEN_FILE; i++) table[i] = NULL;
}

void FileSystem::registerOpenFileTable(OpenFile** table, unsigned int tid){
    this->init_table(table);

    file_table_t * fileTable =  this->ThreadsFilesTable; // get head

    while(fileTable->next != NULL) fileTable = fileTable->next;// go at the end of linked list

    fileTable->next = (file_table_t*) malloc(sizeof(file_table_t));
    fileTable->next->thread_table = table;
    fileTable->next->tid = tid;
}

bool FileSystem::unregisterOpenFileTable(unsigned int tid){
    file_table_t * fileTable =  this->ThreadsFilesTable; // get head

    while(fileTable->next != NULL && fileTable->next->tid != tid) fileTable = fileTable->next;// go at the end of linked list
    if(fileTable->next == NULL) return false;

    for (int i = 0; i < MAX_OPEN_FILE; i++){
        if(fileTable->next->thread_table[i]->isOpenByOthers()) {
            fileTable->next->thread_table[i]->remove_seek(tid);
            continue;
        }
        delete fileTable->next->thread_table[i];
    }

    file_table_t* nnext = fileTable->next->next;
    delete fileTable->next;
    fileTable->next = nnext; //remove element

    return true;
}