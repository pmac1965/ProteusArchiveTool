/**
 *  Copyright 2016 Redcliffe Interactive
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */


#pragma once


// Basic types
typedef   signed char       s8;             // Signed 8 bit
typedef unsigned char       u8;             // Unsigned 8 bit
typedef   signed short      s16;            // Signed 16 bit
typedef unsigned short      u16;            // Unsigned 16 bit
typedef   signed int        s32;            // Signed 32 bit
typedef unsigned int        u32;            // Unsigned 32 bit
typedef   signed __int64    s64;            // Signed 64 bit
typedef unsigned __int64    u64;            // Unsigned 64 bit
typedef float               f32;            // 32 bit float
typedef double              f64;            // 64 bit float


enum
{
    COMPRESSION_TYPE_NONE,                  // Not compressed
    COMPRESSION_TYPE_ZLIB,                  // Using zlib?
};


typedef struct FatHeader
{
    u32 entries;                            // The number of archive entries
    u32 size;                               // The size of the binary file (Including header)
    u32 magic1;                             // File identifier
    u32 magic2;                             // File identifier

} FatHeader;


// Used to convert 4 ascii characters into an identifier.
#define MAKE4(a,b,c,d)      (((a) << 24) +  ((b) << 16) +  ((c) << 8) +  (d))


// Used to uniquely identify an engine file.
#define MAGIC1              MAKE4('p', 'r', 'o', 't')
#define MAGIC2              MAKE4('a', 'r', 'c', 'h')

// Each archive entry is store as this block of data
typedef struct ArcEntry
{
    u32     hash;                           // The hashed filename of the entry
    u32     offset;                         // Offset into the archive
    u32     filesize;                       // The uncompressed filesize of the entry
    u32     compressedSize;                 // The compressed filesize of the entry
    u8      compressed;                     // 0 == uncompressed 1 == compressed
    u8      compressionType;                // COMPRESSION_TYPE_NONE or COMPRESSION_TYPE_ZLIB
    u8      exp0;                           // Expansion purposes (Free to use)
    u8      exp1;                           // Expansion purposes (Free to use)
    char    filename[260];                  // The filename


    bool operator < (const ArcEntry& rhs)
    {
        return hash < rhs.hash;
    }

} ArcEntry;
