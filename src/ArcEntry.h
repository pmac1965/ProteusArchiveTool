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
typedef   signed char       s8;
typedef unsigned char       u8;
typedef   signed short      s16;
typedef unsigned short      u16;
typedef   signed int        s32;
typedef unsigned int        u32;
typedef   signed __int64    s64;
typedef unsigned __int64    u64;
typedef float               f32;
typedef double              f64;


enum
{
    COMPRESSION_TYPE_NONE,
    COMPRESSION_TYPE_ZLIB,      // Using zlib?
};


typedef struct FatHeader
{
    u32 entries;
    u32 size;
    u32 magic1;
    u32 magic2;

} FatHeader;


// Used to convert 4 ascii characters into an identifier.
#define MAKE4(a,b,c,d)      (((a) << 24) +  ((b) << 16) +  ((c) << 8) +  (d))


// Used to uniquely identify an engine file.
#define MAGIC1              MAKE4('p', 'r', 'o', 't')
#define MAGIC2              MAKE4('a', 'r', 'c', 'h')


typedef struct ArcEntry
{
    u32     hash;
    u32     offset;
    u32     filesize;
    u32     compressedSize;
    u8      compressed;
    u8      compressionType;
    u8      exp0;
    u8      exp1;
    char    filename[260];


    bool operator < (const ArcEntry& rhs)
    {
        return hash < rhs.hash;
    }

} ArcEntry;
