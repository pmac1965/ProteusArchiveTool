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


#include <stdio.h>
#include "ShowUsage.h"
#include <stdlib.h>


void ShowUsage()
{
    static const char* text = 
    "-------------------------------------------------------------------------------\n"
    "(P)roteus (A)rchive (T)ool                                                     \n"
    "Copyright 2016 Redcliffe Interactive. All rights reserved.                     \n"
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
    "    -lc    Convert all filenames to lowercase. Defaults to using files case.   \n"
    "    -uc    Convert all filenames to uppercase. Defaults to using files case.   \n"
    "    -h     Show this help information.                                         \n"
    "    -v     Show the version number.                                            \n"
    "    -verb  Enable verbose output.                                              \n"
    "    -c     Enable file compression.                                            \n"
    "                                                                               \n"
    "Usage example:                                                                 \n"
    "                                                                               \n"
    "    pat -i [directory] -o [output-name] -c                                     \n"
    "-------------------------------------------------------------------------------\n";

    printf(text);
}
