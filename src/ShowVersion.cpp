// ----------------------------------------------------------------------------
//
// File: ShowVersion.cpp
//
//      Description     - Shows app version.
//      Author          - Paul Michael McNab.
//      Copyright       - Copyright Redcliffe Interactive. All rights reserved.
//
// ----------------------------------------------------------------------------


#include <stdio.h>
#include "ShowUsage.h"
#include <stdlib.h>


// 1.0.0 - Initial version
// 1.0.1 - Added zlib 1.2.8, Added icon
// 1.1.0 - Changed copyright to Redcliffe Interactive from Paul Michael McNab.


void ShowVersion()
{
    static const char* text = 
    "-------------------------------------------------------------------------------\n"
    "(P)roteus (A)rchive (T)ool                                                     \n"
    "Copyright 2014 Redcliffe Interactive. All rights reserved.                     \n"
    "Version 1.1.0                                                                  \n"
    "Uses zlib 1.2.8                                                                \n"
    "Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler                        \n"
    "-------------------------------------------------------------------------------\n";
    printf(text);
}
