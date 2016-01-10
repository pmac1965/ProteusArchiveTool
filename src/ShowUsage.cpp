// ----------------------------------------------------------------------------
//
// File: ShowUsage.cpp
//
//      Description     - Shows app usage.
//      Author          - Paul Michael McNab.
//      Copyright       - Copyright Redcliffe Interactive. All rights reserved.
//
// ----------------------------------------------------------------------------


#include <stdio.h>
#include "ShowUsage.h"
#include <stdlib.h>


void ShowUsage()
{
    static const char* text = 
    "-------------------------------------------------------------------------------\n"
    "(P)roteus (A)rchive (T)ool                                                     \n"
    "Copyright 2014 Redcliffe Interactive. All rights reserved.                     \n"
    "Uses zlib 1.2.8                                                                \n"
    "Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler                        \n"
    "                                                                               \n"
    "This tool archives the contents of a directory. The tool outputs two files.    \n"
    "The archive description file, and the archive file.                            \n"
    "                                                                               \n"
    "Zero length files will be not be added to the archive.                         \n"
    "                                                                               \n"
    "Arguments:                                                                     \n"
    "                                                                               \n"
    "    -i      The path of the directory to add to the archive.                   \n"
    "    -o      The name of the two files to create. This name should not include  \n"
    "            an extension. Extensions will be added by the tool.                \n"
    "                                                                               \n"
    "Optional arguments:                                                            \n"
    "                                                                               \n"
    "    -lc    Convert all filenames to lowercase. Defaults to case sensitive.     \n"
    "    -uc    Convert all filenames to uppercase. Defaults to case sensitive.     \n"
    "    -h     Show this help information.                                         \n"
    "    -v     Show the version number.                                            \n"
    "    -verb  Enable verbose output.                                              \n"
    "    -c     Enable file compression.                                            \n"
    "                                                                               \n"
    "Usage example:                                                                 \n"
    "                                                                               \n"
    "    pat -i [directory] -o [output-name]                                        \n"
    "-------------------------------------------------------------------------------\n";

    printf(text);
}
