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
#include <stdlib.h>
#include "ShowUsage.h"


// 1.0.0 - Initial version
// 1.0.1 - Added zlib 1.2.8, Added icon
// 1.1.0 - Changed copyright to Redcliffe Interactive from Paul Michael McNab.
// 1.2.0 - Moved to github. Made code open source.


namespace
{
    int versionMajor    = 1;
    int versionMinor    = 2;
    int versionRevision = 0;
}


// ----------------------------------------------------------------------------
// Shows application version
// ----------------------------------------------------------------------------
void ShowVersion()
{
    static const char* text = 
    "-------------------------------------------------------------------------------\n"
    "(P)roteus (A)rchive (T)ool                                                     \n"
    "Copyright 2016 Redcliffe Interactive. All rights reserved.                     \n"
    "Version %i.%i.%i                                                               \n"
    "Uses zlib 1.2.8                                                                \n"
    "Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler                        \n"
    "-------------------------------------------------------------------------------\n";


    printf(text, versionMajor, versionMinor, versionRevision);
}
