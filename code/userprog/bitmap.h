/**
 * @file bitmap.h
 * @brief Data structures defining a bitmap
 * @author Olivier Hureau,  Hugo Feydel , Julien ALaimo
 *      Data structures defining a bitmap -- an array of bits each of which
 *      can be either on or off.
 *
 *      Represented as an array of unsigned integers, on which we do
 *      modulo arithmetic to find the bit we are interested in.
 *
 *      The bitmap can be parameterized with with the number of bits being
 *      managed.
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation
* of liability and disclaimer of warranty provisions.
*/
#ifndef BITMAP_H
#define BITMAP_H

#include "copyright.h"
#include "utility.h"
#include "openfile.h"

/// Definitions helpful for representing a bitmap as an array of integers
#define BitsInByte 	8
#define BitsInWord 	32

/// The following class defines a "bitmap" -- an array of bits,
/// each of which can be independently set, cleared, and tested.
///
/// Most useful for managing the allocation of the elements of an array --
/// for instance, disk sectors, or main memory pages.
/// Each bit represents whether the corresponding sector or page is
/// in use or free.
class BitMap
{
  public:
    /// Initialize a bitmap, with "nitems" bits
    BitMap (int nitems);

    /// initially, all bits are cleared.
     ~BitMap ();		// De-allocate bitmap

    /// Set the "nth" bit
    void Mark (int which);

    /// Clear the "nth" bit
    void Clear (int which);

    /// Is the "nth" bit set?
    bool Test (int which);

    /// Return the # of a clear bit, and as a side
    int Find ();
    // effect, set the bit. 
    // If no bits are clear, return -1.

    /// Return the number of clear bits
    int NumClear ();
    /// Print contents of bitmap
    void Print ();

    // These aren't needed until FILESYS, when we will need to read and
    // write the bitmap to a file
    /// fetch contents from disk
    void FetchFrom (OpenFile * file);

    /// write contents to disk
    void WriteBack (OpenFile * file);

  private:
    /// number of bits in the bitmap
    int numBits;
    /// number of words of bitmap storage
    int numWords;
    /// (rounded up if numBits is not a
    ///  multiple of the number of bits in
    ///  a word)
    unsigned int *map;		// bit storage
};

#endif // BITMAP_H
