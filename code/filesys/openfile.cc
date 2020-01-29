/// @file  openfile.c                                    
/// @brief Routines to manage an open Nachos file.
/// @author Olivier Hureau,  Hugo Feydel , Julien ALaimo 
/// openfile.cc 
///	Routines to manage an open Nachos file.  As in UNIX, a
///	file must be open before we can read or write to it.
///	Once we're all done, we can close it (in Nachos, by deleting
///	the OpenFileTable data structure).
///
///	Also as in UNIX, for convenience, we keep the file header in
///	memory while the file is open.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
/// All rights reserved.  See copyright.h for copyright notice and limitation 
/// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "filehdr.h"
#include "openfile.h"
#include "system.h"

#include <strings.h> /* for bzero */

///
/// OpenFileTable::OpenFileTable
/// 	Open a Nachos file for reading and writing.  Bring the file header
///	into memory while the file is open.
///
///	@param "sector" -- the location on disk of the file header for this file
///

OpenFile::OpenFile(int sector)
{ 
    hdr = new FileHeader;
    hdr->FetchFrom(sector);
    this->seek_tid_list = (tuple_t *) malloc(sizeof(tuple_t));
    this->seek_tid_list->tid = 0;
    this->seek_tid_list->seekPosition = 0;
    this->seek_tid_list->next = NULL;
}

///
/// OpenFileTable::~OpenFileTable
/// 	Close a Nachos file, de-allocating any in-memory data structures.
///

OpenFile::~OpenFile(){
    delete seek_tid_list;
    delete hdr;
}

///
/// OpenFileTable::Seek
/// 	Change the current location within the open file -- the point at
///	which the next Read or Write will start from.
///
///	@param "position" -- the location within the file for the next Read/Write
///
void OpenFile::Seek(int position, unsigned int tid) {
    this->set_seek_position(tid, position);
}

///
/// OpenFileTable::Read/Write
/// 	Read/write a portion of a file, starting from seekPosition.
///	@return Return the number of bytes actually written or read, and as a
///	side effect, increment the current position within the file.
///
///	Implemented using the more primitive ReadAt/WriteAt.
///
///	@param "into" -- the buffer to contain the data to be read from disk
///	@param "from" -- the buffer containing the data to be written to disk
///	@param "numBytes" -- the number of bytes to transfer
int OpenFile::Read(char *into, int numBytes, unsigned int tid){
   int result = ReadAt(into, numBytes, this->get_seek_position(tid));
   this->set_seek_position(tid, this->get_seek_position(tid)+result);
   return result;
}

bool OpenFile::isOpenByOthers(){
    return this->seek_tid_list->next != NULL;
}

/// OpenFileTable::add_seek add a seek to a openfile
/// Allow multi threads to access to the same openfile with separated pointer
/// \param tid the thread tid
//
void OpenFile::add_seek(unsigned int tid){
    tuple_t * list = this->seek_tid_list; //get head

    while(list->next != NULL) list = list->next;

    list->next = (tuple_t*) malloc(sizeof(tuple_t));
    list->next->tid = tid;
    list->next->seekPosition = 0;
    list->next->next = NULL;
}

/// OpenFileTable::remove_seek remove a seek to a openfile
/// Allow multi threads to access to the same openfile with separated pointer
/// \param tid the thread tid
// [(23,0),[42,1)]
bool OpenFile::remove_seek(unsigned int tid){
    tuple_t* list = this->seek_tid_list; //get head

    //TODO CODE DUPLICATE
    if(list->tid == tid){
        tuple_t* next = list->next;
        delete list;
        this->seek_tid_list = next; //remove element
        return true;
    }

    while(list->next != NULL && list->next->tid != tid)list = list->next;
    if(list->next == NULL) return false;

    tuple_t* nnext = list->next->next;
    delete list->next;
    list->next = nnext; //remove element

    return true;
}
/// OpenFileTable::set_seek_position
/// set the seek positon.
/// \param tid the thread tid
/// \param seekPosition the new seek position
void OpenFile::set_seek_position(unsigned int tid, int seekPosition){
    tuple_t *list = this->seek_tid_list;

    while (list != NULL){
        if(list->tid == tid) list->seekPosition = seekPosition;
        list = list->next;
    }
}

/// OpenFileTable::get_seek_position
/// \param tid the thread tid
/// \return the seek positon
int OpenFile::get_seek_position(unsigned int tid){
    tuple_t *list = this->seek_tid_list;

    while (list != NULL){
        if(list->tid == tid) return list->seekPosition;
        list = list->next;
    }
    return -1;
}

/// OpenFileTable::Write into a file
/// \param into
/// \param numBytes
/// \param tid
/// \return
int OpenFile::Write(const char *into, int numBytes, unsigned int tid){
    int result = WriteAt(into, numBytes, this->get_seek_position(tid));
    this->set_seek_position(tid, this->get_seek_position(tid)+result);
    return result;
}

//
/// OpenFileTable::ReadAt/WriteAt
/// 	Read/write a portion of a file, starting at "position".
///	Return the number of bytes actually written or read, but has
///	no side effects (except that Write modifies the file, of course).
///
///	There is no guarantee the request starts or ends on an even disk sector
///	boundary; however the disk only knows how to read/write a whole disk
///	sector at a time.  Thus:
///
///	For ReadAt:
///	   We read in all of the full or partial sectors that are part of the
///	   request, but we only copy the part we are interested in.
///	For WriteAt:
///	   We must first read in any sectors that will be partially written,
///	   so that we don't overwrite the unmodified portion.  We then copy
///	   in the data that will be modified, and write back all the full
///	   or partial sectors that are part of the request.
///
///	@param "into" -- the buffer to contain the data to be read from disk
///	@param "from" -- the buffer containing the data to be written to disk
///	@param "numBytes" -- the number of bytes to transfer
///	@param "position" -- the offset within the file of the first byte to be
///			read/written
///

int OpenFile::ReadAt(char *into, int numBytes, int position){

    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    char *buf;

    if ((numBytes <= 0) || (position >= fileLength))
    	return 0; 				// check request
    if ((position + numBytes) > fileLength)		
	numBytes = fileLength - position;
    DEBUG('f', "Reading %d bytes at %d, from file of length %d.\n", 	
			numBytes, position, fileLength);

    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    // read in all the full and partial sectors that we need
    buf = new char[numSectors * SectorSize];

    for (i = firstSector; i <= lastSector; i++)
        synchDisk->ReadSector(hdr->ByteToSector(i * SectorSize), &buf[(i - firstSector) * SectorSize]);

    // copy the part we want
    bcopy(&buf[position - (firstSector * SectorSize)], into, numBytes);
    delete [] buf;
    return numBytes;
}

int OpenFile::WriteAt(const char *from, int numBytes, int position){
    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    bool firstAligned, lastAligned;
    char *buf;

    if ((numBytes <= 0) || (position >= fileLength))
	return 0;				// check request
    if ((position + numBytes) > fileLength)
	numBytes = fileLength - position;
    DEBUG('f', "Writing %d bytes at %d, from file of length %d.\n", 	
			numBytes, position, fileLength);

    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    buf = new char[numSectors * SectorSize];

    firstAligned = (position == (firstSector * SectorSize));
    lastAligned = ((position + numBytes) == ((lastSector + 1) * SectorSize));

// read in first and last sector, if they are to be partially modified
    if (!firstAligned)
        ReadAt(buf, SectorSize, firstSector * SectorSize);	
    if (!lastAligned && ((firstSector != lastSector) || firstAligned))
        ReadAt(&buf[(lastSector - firstSector) * SectorSize], 
				SectorSize, lastSector * SectorSize);	

// copy in the bytes we want to change 
    bcopy(from, &buf[position - (firstSector * SectorSize)], numBytes);

// write modified sectors back
    for (i = firstSector; i <= lastSector; i++)	
        synchDisk->WriteSector(hdr->ByteToSector(i * SectorSize), 
					&buf[(i - firstSector) * SectorSize]);
    delete [] buf;
    return numBytes;
}

///
/// OpenFileTable::Length
/// 	@return Return the number of bytes in the file.
///

int OpenFile::Length() {
    return hdr->FileLength(); 
}

int OpenFile::get_sector() {
    return ((FileHeader* )this->hdr)->get_sector(0);
}

bool OpenFile::isdir() {
    return (this->hdr->type == d);
}
