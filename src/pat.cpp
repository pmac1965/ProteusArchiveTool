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


#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include "ShowUsage.h"
#include "ShowVersion.h"
#include "ArcEntry.h"
#include "zlib/zlib.h"


enum
{
    COMPRESS_FAILED   = -1,
    COMPRESS_SUCCESS,
    COMPRESS_LARGER,
};


// Enable run-time memory check for debug builds.
#if defined(_DEBUG)
#include <crtdbg.h>
#endif


// Macros
#define ARRAY_SIZE(a)           (sizeof((a)) / sizeof(*(a)))
//#define PAT_SHOW_FILES
//#define EXAMPLE_CODE


#ifdef EXAMPLE_CODE
void Find();
#endif


// Rounds a number up by the specified amount.
#define ROUND_UP(number, amount)      (((u32)(number) + (amount) - 1)  &  ~((amount) - 1))


// Local data
namespace
{
    HANDLE   hConsole    = INVALID_HANDLE_VALUE;
    bool     lowerCase   = false;
    bool     upperCase   = false;
    bool     gotInput    = false;
    bool     gotOutput   = false;
    bool     verbose     = false;
    bool     crushData   = false;

    _TCHAR   inputDirectory      [MAX_PATH];
    _TCHAR   outputFilename      [MAX_PATH];
    _TCHAR   inputDirectory_Full [MAX_PATH];
    _TCHAR   outputFilename_Full [MAX_PATH];
    _TCHAR   outputFilename_Fat  [MAX_PATH];
    _TCHAR   outputFilename_Arc  [MAX_PATH];

    std::list<ArcEntry>  filesToAdd;
}


// Local functions
void UnknownCommand(const _TCHAR* argv);
bool GetArgument(const _TCHAR** argv, int index, int total, _TCHAR *data);
bool CheckArguments();
bool ValidateDirectory(const _TCHAR *filename);
bool FileExist(const _TCHAR *filename);
bool ValidateFile(const _TCHAR *filename);
bool ValidateNotDirectory(const _TCHAR *filename);
void ScanDirectory(const _TCHAR *filename);
void AddFile(WIN32_FIND_DATAW &fd, const _TCHAR *parentDirectory);
void DebugShowLastError();
void CreateEntry(WIN32_FIND_DATAW &fd, const _TCHAR *filename);
bool WriteArchive();
u32  StringHash(const char* string);
int  CompressData(const Bytef *data, uLong dataSize, uLong &dataOutSize, u8 **dataOut);
void StringReplaceChar(char *string, char search, char replace);


// ----------------------------------------------------------------------------
// Cleans up.
// ----------------------------------------------------------------------------
void fnExit(void)
{
#ifdef PAT_SHOW_FILES
    if (verbose)
    {
        if (filesToAdd.size() > 0)
        {
            printf("-------------------------------------------------------------------------------\n");
            printf("Files in archive\n");
            printf("-------------------------------------------------------------------------------\n");

            std::list<ArcEntry>::iterator it  = filesToAdd.begin();
            std::list<ArcEntry>::iterator end = filesToAdd.end();
            for(;it != end; ++it)
            {
                // Display entries on exit.
                printf("Size: %*i : %s\n", 10, (*it).filesize, (*it).filename);
            }
        }
    }
#endif

    filesToAdd.clear();
}


// ----------------------------------------------------------------------------
// App entry.
// ----------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
    // Enable run-time memory check for debug builds.
    #if defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    #endif

    // Set cleanup
    atexit(fnExit);

    // Get console
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE)
    {
        SetConsoleTitle(TEXT("Proteus Archive Tool"));
    }

    // No args?
    if (argc == 1)
    {
        ShowUsage();
        return 1;
    }

    // Parse args
    int count = argc - 1;
    for (int i=1; i<=count; i++)
    {
        // Is command
        if (argv[i][0] == L'-')
        {
            switch(argv[i][1])
            {
            // Show version?
            case L'v':
                if (_tcsicmp(L"-v", argv[i]) == 0)
                {
                    ShowVersion();
                    return 0;
                }
                else if (_tcsicmp(L"-verb", argv[i]) == 0)
                {
                    verbose = true;
                }
                else
                {
                    UnknownCommand(argv[i]);
                    return 1;
                }
                break;

            // Show help?
            case L'h':
                if (_tcsicmp(L"-h", argv[i]) == 0)
                {
                    ShowUsage();
                    return 0;
                }
                else
                {
                    UnknownCommand(argv[i]);
                    return 1;
                }
                break;

            // Lower case file names.
            case L'l':
                if (_tcsicmp(L"-lc", argv[i]) == 0)
                {
                    lowerCase = true;
                }
                else
                {
                    UnknownCommand(argv[i]);
                    return 1;
                }
                break;

            // Upper case file names.
            case L'u':
                if (_tcsicmp(L"-uc", argv[i]) == 0)
                {
                    upperCase = true;
                }
                else
                {
                    UnknownCommand(argv[i]);
                    return 1;
                }
                break;

            // Compress?
            case L'c':
                if (_tcsicmp(L"-c", argv[i]) == 0)
                {
                    crushData = true;
                }
                else
                {
                    UnknownCommand(argv[i]);
                    return 1;
                }
                break;

            // Input directory
            case L'i':
                if (_tcsicmp(L"-i", argv[i]) == 0)
                {
                    if (GetArgument((const _TCHAR **)argv, i, count, inputDirectory) == false)
                    {
                        return 1;
                    }
                    else
                    {
                        gotInput = true;

                        // Bypass arguments value.
                        i++;
                    }
                }
                else
                {
                    UnknownCommand(argv[i]);
                    return 1;
                }
                break;

            // Output filename
            case L'o':
                if (_tcsicmp(L"-o", argv[i]) == 0)
                {
                    if (GetArgument((const _TCHAR **)argv, i, count, outputFilename) == false)
                    {
                        return 1;
                    }
                    else
                    {
                        gotOutput = true;

                        // Bypass arguments value.
                        i++;
                    }
                }
                else
                {
                    UnknownCommand(argv[i]);
                    return 1;
                }
                break;

            default:
                UnknownCommand(argv[i]);
                return 1;
            }
        }
        else
        {
            UnknownCommand(argv[i]);
            return 1;
        }
    }

    // Check arguments
    if (!CheckArguments())
    {
        return 1;
    }
       
    // Get full paths
    if (GetFullPathName(outputFilename, MAX_PATH, outputFilename_Full, NULL) == 0)
    {
        printf("Failed to get full path name for:\n%ls\n", outputFilename);
        return 1;
    }

    if (GetFullPathName(inputDirectory, MAX_PATH, inputDirectory_Full, NULL) == 0)
    {
        printf("Failed to get full path name for:\n%ls\n", inputDirectory);
        return 1;
    }

    // Validate input/output sources.
    if (!ValidateDirectory(inputDirectory_Full))
    {
        return 1;
    }

    // Copy to make output filenames
    _tcscpy_s(outputFilename_Fat, MAX_PATH, outputFilename_Full);
    _tcscpy_s(outputFilename_Arc, MAX_PATH, outputFilename_Full);

    // Check output names are not directories
    if (!ValidateNotDirectory(outputFilename_Fat))
    {
        return 1;
    }

    if (!ValidateNotDirectory(outputFilename_Arc))
    {
        return 1;
    }

    // Add extensions
    _tcscat_s(outputFilename_Fat, MAX_PATH, L".fat");
    _tcscat_s(outputFilename_Arc, MAX_PATH, L".arc");

    // Validate.
    if (!ValidateFile(outputFilename_Fat))
    {
        return 1;
    }

    if (!ValidateFile(outputFilename_Arc))
    {
        return 1;
    }

    // Scan
    ScanDirectory(inputDirectory_Full);

    // Got files to add?
    if (filesToAdd.size() == 0)
    {
        printf("Didn't find any files to add to archive\n");
        return 1;
    }

    if (!WriteArchive())
        return 1;

    // Test example code
#ifdef EXAMPLE_CODE
    Find();
#endif

	return 0;
}


// ------------------------------------------------------------------------
// Unknown command handler.
// ------------------------------------------------------------------------
void UnknownCommand(const _TCHAR* argv)
{
    printf("Unknown command line parameter: %ls\n", argv);
}


// ------------------------------------------------------------------------
// Gets the argument for a command
//
// argv  == The arguments array.
// index == The current index into the argument values array
// total == The total number of argument values
// data  == The storage string
// ------------------------------------------------------------------------
bool GetArgument(const _TCHAR** argv, int index, int total, _TCHAR *data)
{
    // Do we have enough command line arguments?
    if (index + 1 > total)
    {
        printf("No data for argument: %ls\n", argv[index]);
        return false;
    }

    // Is the argument empty?
    _TCHAR *pArg = (_TCHAR*)argv[index + 1];
    if (*pArg == L'\0')
    {
        printf("You can't have empty data for argument: %ls\n", argv[index]);
        return false;
    }

    // Is the argument too long?
    size_t size = _tcslen(pArg);
    if (size >= MAX_PATH)
    {
        printf("The data is too long for argument : %ls\n", argv[index]);
        return false;
    }

    // Store the argument
    _tcscpy_s(data, MAX_PATH, pArg);
    return true;
}


// ------------------------------------------------------------------------
// Check the arguments
// ------------------------------------------------------------------------
bool CheckArguments()
{
    if (lowerCase && upperCase)
    {
        printf("You can't use both case conversion options\n");
        return false;
    }

    if (!gotInput)
    {
        printf("No input directory specified\n");
        return false;
    }

    if (!gotOutput)
    {
        printf("No output filename specified\n");
        return false;
    }

    return true;
}


// ------------------------------------------------------------------------
// Validates that we can use the directory.
// ------------------------------------------------------------------------
bool ValidateDirectory(const _TCHAR *filename)
{
    // Get attributes.
    DWORD attribs = GetFileAttributes(filename);
    if (attribs == 0xffffffff)
    {
        printf("Failed to acquire file attributes for:\n%ls\n", filename);
        return false;
    }

    // Directory.
    if ((attribs & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
    {
        printf("Input source is not a directory:\n%ls\n", filename);
        return false;
    }

    return true;
}


// ------------------------------------------------------------------------
// Determines if the passed file exists.
// ------------------------------------------------------------------------
bool FileExist(const _TCHAR *filename)
{
    if (filename && *filename)
    {
        FILE *fp = 0;        
        _tfopen_s(&fp, filename, L"r");
        if (fp)
        {
            fclose(fp);
            return true;
        }
    }

    return false;
}


// ------------------------------------------------------------------------
// Validates that we can use the file
// ------------------------------------------------------------------------
bool ValidateFile(const _TCHAR *filename)
{
    static const DWORD flags[] =
    {
        FILE_ATTRIBUTE_HIDDEN,
        FILE_ATTRIBUTE_SYSTEM,
        FILE_ATTRIBUTE_DIRECTORY,
        FILE_ATTRIBUTE_TEMPORARY,
        FILE_ATTRIBUTE_OFFLINE,
        FILE_ATTRIBUTE_COMPRESSED,
    };


    // Does the file exist?
    if (FileExist(filename))
    {
        // Get attributes.
        DWORD attribs = GetFileAttributes(filename);
        if (attribs == 0xffffffff)
        {
            printf("Failed to acquire file attributes for:\n'%ls'\n", filename);            
            return false;
        }

        // Any unsuitable attributes?
        for (int i=0; i<ARRAY_SIZE(flags); i++)
        {
            if (attribs & flags[i])
            {
                printf("Unable to use:\n%ls\n", filename);
                return false;
            }
        }

        // Read only?
        if ((attribs & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
        {
            printf("Unable to use file as it is read only:\n%ls\n", filename);
            return false;
        }
    }
    else
    {
        // Create a temp
        FILE *fp = NULL;  
        _tfopen_s(&fp, filename, L"wb");
        if (fp)
        {
            fclose(fp);
        }
        else
        {
            printf("Failed to create output file.");
            return false;
        }
    }

    return true;
}


// ------------------------------------------------------------------------
// Validates that we can use the file
// ------------------------------------------------------------------------
bool ValidateNotDirectory(const _TCHAR *filename)
{
    // Does the file exist?
    DWORD attribs = GetFileAttributes(filename);
    if (attribs == 0xffffffff)
    {
        return true;
    }

    // Directory?
    if ((attribs & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
    {
        printf("Unable to use output name as it is directory\n");
        return false;
    }

    return true;
}


// ------------------------------------------------------------------------
// Scans the directory.
// ------------------------------------------------------------------------
void ScanDirectory(const _TCHAR *filename)
{
    // Create search path and base path.
    _TCHAR  filenameParent  [MAX_PATH];
    _TCHAR  filenameDir     [MAX_PATH];

    _tcscpy_s(filenameParent, MAX_PATH, filename);
    _tcscpy_s(filenameDir,    MAX_PATH, filename);
    _tcscat_s(filenameDir,    MAX_PATH, L"\\*.*");


    // Find first file.
    WIN32_FIND_DATAW fd;
    HANDLE handle = FindFirstFile(filenameDir, &fd);

    if (handle == INVALID_HANDLE_VALUE)
    {
        printf("Directory scan failed.\n");
        DebugShowLastError();
        return;
    }
    else
    {
        // Add the first file?
        AddFile(fd, filenameParent);

        // Add next
        BOOL result = FALSE;
        do
        {
            result = FindNextFile(handle, &fd);

            if (result == TRUE)
            {
                AddFile(fd, filenameParent);
            }
        }
        while(result == TRUE);
    }

    FindClose(handle);
}


// ------------------------------------------------------------------------
// Checks for a file we can use.
// ------------------------------------------------------------------------
void AddFile(WIN32_FIND_DATAW &fd, const _TCHAR *parentDirectory)
{
    static const DWORD flags[] =
    {
        FILE_ATTRIBUTE_HIDDEN,
        FILE_ATTRIBUTE_SYSTEM,
        FILE_ATTRIBUTE_DIRECTORY,
        FILE_ATTRIBUTE_TEMPORARY,
        FILE_ATTRIBUTE_OFFLINE,
        FILE_ATTRIBUTE_COMPRESSED,
    };


    // If its a directory, then scan it.
    if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
    {
        // Skip system directories.
        if (_tcsicmp(L".", fd.cFileName) == 0 || _tcsicmp(L"..", fd.cFileName) == 0) 
        {
            return;
        }


        // Create the complete path
        _TCHAR  directory[MAX_PATH];

        _tcscpy_s(directory, MAX_PATH, parentDirectory);
        _tcscat_s(directory, MAX_PATH, L"\\");
        _tcscat_s(directory, MAX_PATH, fd.cFileName);

        ScanDirectory(directory);
        return;
    }

    // Any unsuitable attributes?
    for (int i=0; i<ARRAY_SIZE(flags); i++)
    {
        if (fd.dwFileAttributes & flags[i])
        {
            printf("Skipping file: %ls\n", fd.cFileName);
            return;
        }
    }

    // File empty?
    if (fd.nFileSizeLow == 0 && fd.nFileSizeHigh == 0)
    {
        return;
    }

    // File too large.
    if (fd.nFileSizeHigh != 0)
    {
        printf("Files larger than %lu bytes are not supported: %ls\n", 0xffffffff, fd.cFileName);
        return;
    }

    // Create the complete path
    {
        _TCHAR  filename[MAX_PATH];

        _tcscpy_s(filename, MAX_PATH, parentDirectory);
        _tcscat_s(filename, MAX_PATH, L"\\");
        _tcscat_s(filename, MAX_PATH, fd.cFileName);

        // Store file.
        CreateEntry(fd, filename);
    }
}


// ------------------------------------------------------------------------
// Displays the last windows error message.
// ------------------------------------------------------------------------
void DebugShowLastError()
{
    DWORD dw = GetLastError(); 
    if (dw != 0)
    {
        LPVOID lpMsgBuf = NULL;

        FormatMessageW
        (
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM     |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
        );

        printf("============================================================\n");
        printf("Last error: %s", (char *)lpMsgBuf);
        printf("============================================================\n");

        if (lpMsgBuf)
        {
          LocalFree(lpMsgBuf);
        }
    }
}


// ----------------------------------------------------------------------------
// Turns a string into a number.
// ----------------------------------------------------------------------------
u32 StringHash(const char* string)
{
    u32 hash = 0;

    while(*string)
    {
        hash += *string;
        hash *= *string++;
    }

    return hash;
}


// ------------------------------------------------------------------------
// Creates an archive entry.
// ------------------------------------------------------------------------
void CreateEntry(WIN32_FIND_DATAW &fd, const _TCHAR *filename)
{
    try
    {
        // Create entry.
        ArcEntry entry;

        // No compression
        entry.compressed       = false;
        entry.compressionType  = COMPRESSION_TYPE_NONE;
        entry.compressedSize   = 0;
        entry.exp0             = 0;
        entry.exp1             = 0;
        entry.filesize         = fd.nFileSizeLow;
        entry.hash             = 0;
        entry.offset           = 0;

        // Save filename as ansi. The simple way, no complex conversion functions here.
        #ifdef  _UNICODE
            sprintf_s(entry.filename, MAX_PATH, "%ls", filename);
        #else
            sprintf_s(entry.filename, MAX_PATH, "%s", filename);
        #endif

        filesToAdd.push_back(entry);
    }
    catch(...)
    {
        printf("Memory alloc failed.");
        exit(1);
    }
}


// ------------------------------------------------------------------------
// Create the fat
// ------------------------------------------------------------------------
bool WriteArchive()
{
    // Create the header
    FatHeader   header;
    header.magic1   = MAGIC1;
    header.magic2   = MAGIC2;
    header.size     = sizeof(FatHeader) + (sizeof(ArcEntry) * filesToAdd.size());
    header.entries  = filesToAdd.size();


    // Open the files
    FILE *fp_fat = NULL;  
    FILE *fp_arc = NULL;      
    _tfopen_s(&fp_fat, outputFilename_Fat, L"wb");
    _tfopen_s(&fp_arc, outputFilename_Arc, L"wb");  
    if (fp_fat && fp_arc)
    {
        // Write the FAT header.
        size_t bytes = fwrite(&header, 1, sizeof(FatHeader), fp_fat);


        // Create a copy of the files to add.
        std::list<ArcEntry>entries;

        char work[MAX_PATH];
        char temp[MAX_PATH];
        int  len = _tcslen(inputDirectory_Full);


        std::list<ArcEntry>::iterator it  = filesToAdd.begin();
        std::list<ArcEntry>::iterator end = filesToAdd.end();
        for(;it != end; ++it)
        {
            ArcEntry entry;

            // Make copy of filename and remove input path.
            sprintf_s(work, MAX_PATH, "%s", (*it).filename);
            strcpy_s(temp, MAX_PATH, &work[len + 1]);

            if (upperCase)
            {
                _strupr_s(temp, MAX_PATH);
            }
            else if (lowerCase)
            {
                _strlwr_s(temp, MAX_PATH);
            }

            // Ensure same slashes.
            StringReplaceChar(temp, '\\', '/');

            // Create entry
            sprintf_s(entry.filename, MAX_PATH, "%s", (*it).filename);

            entry.hash              = StringHash(temp);                     // Hash using stripped path!
            entry.offset            = 0;
            entry.filesize          = (*it).filesize;
            entry.compressed        = (*it).compressed;
            entry.compressedSize    = (*it).compressedSize;
            entry.compressionType   = (*it).compressionType;

            entries.push_back(entry);
        }


        // Sort for faster searching.
        entries.sort();

        if (verbose)
        {
            printf("-------------------------------------------------------------------------------\n");
            printf("    Offset Compressed     Actual       Hash\n");
            printf("in archive       size   Filesize     Number : File\n");
            printf("-------------------------------------------------------------------------------\n");
        }

        // Write the data.
        {
            u32 offset   = 0;
            u32 filesize = 0;

            std::list<ArcEntry>::iterator it1  = entries.begin();
            std::list<ArcEntry>::iterator end1 = entries.end();
            for(;it1 != end1; ++it1)
            {
                // Open file to archive.
                FILE *fp = NULL;
                fopen_s(&fp, (*it1).filename, "rb");
                if (fp)
                {
                    // Get file size
                    if (fseek(fp, 0, SEEK_END) == 0)
                    {
                        filesize = ftell(fp);
                        rewind(fp);       
                    }


                    // Sanity check - though should not happen as files are filtered.
                    if (filesize == 0)
                    {
                        printf("Empty file found. Cannot complete process.\n");
                        fclose(fp);
                        goto Error;
                    }


                    // Read the data
                    u8 *data = (u8*)malloc(filesize + 4);// + 4 for rounding
                    if (data)
                    {
                        // Read the data
                        size_t bytes = fread(data, 1, filesize, fp);
                        fclose(fp);

                        if (bytes != filesize || bytes != (*it1).filesize)
                        {
                            printf("File has changed size. Cannot complete process.\n");
                            free(data);
                            goto Error;
                        }


                        // Write to the archive
                        if (crushData)
                        {
                            uLong  dataOutSize = 0;
                            u8    *dataOut = NULL;

                            if (CompressData(data, filesize, dataOutSize, &dataOut) == COMPRESS_SUCCESS)
                            {
                                fwrite(dataOut, 1, ROUND_UP(dataOutSize, 4), fp_arc);

                                (*it1).compressed     = true;
                                (*it1).compressedSize = dataOutSize;

                                free(dataOut);
                                dataOut = NULL;
                            }
                            else
                            {
                                //printf("File didn't compress %s > Normal: %i, Compressed: %i\n", (*it1).filename, filesize, dataOutSize);
                                (*it1).compressed     = false;
                                (*it1).compressedSize = 0;
                                fwrite(data, 1, ROUND_UP(filesize, 4), fp_arc);
                            }
                        }
                        else
                        {
                            fwrite(data, 1, ROUND_UP(filesize, 4), fp_arc);
                        }

                        free(data);


                        // Write the entry.
                        ArcEntry entry;


                        // Make copy of filename and remove input path.
                        sprintf_s(work, MAX_PATH, "%s", (*it1).filename);
                        strcpy_s(entry.filename, MAX_PATH, &work[len + 1]);


                        if (upperCase)
                        {
                            _strupr_s(entry.filename, MAX_PATH);
                        }
                        else if (lowerCase)
                        {
                            _strlwr_s(entry.filename, MAX_PATH);
                        }


                        // Ensure same slashes.
                        StringReplaceChar(entry.filename, '\\', '/');


                        // Create entry
                        entry.hash              = StringHash(entry.filename);
                        entry.offset            = offset;
                        entry.filesize          = (*it1).filesize;
                        entry.compressed        = (*it1).compressed;
                        entry.compressedSize    = (*it1).compressedSize;
                        entry.compressionType   = (*it1).compressionType;


                        // Write entry.
                        bytes = fwrite(&entry, 1, sizeof(entry), fp_fat);

                        if (bytes != sizeof(entry))
                        {
                            printf("Failed to write archive entry correctly\n");
                            free(data);
                            goto Error;
                        }


                        if (crushData)
                        {
                            u32 size = 0;

                            if ((*it1).compressed)
                            {
                                size = (*it1).compressedSize;
                            }
                            else
                            {
                                size = (*it1).filesize;
                            }

                            offset += ROUND_UP(size, 4);
                        }
                        else
                        {
                            u32 size = (*it1).filesize;

                            offset += ROUND_UP(size, 4);
                        }

                        if (verbose)
                        {
                           printf("%*i %*i %*i %*x : %s\n", 10, entry.offset,
                                                            10, entry.compressedSize,
                                                            10, entry.filesize,
                                                            10, entry.hash,
                                                            entry.filename);
                        }
                    }
                }
                else
                {
                    printf("Failed to read file into archive.\n%s\n", (*it1).filename);
                    continue;
                }
            }
        }

Error:
        fclose(fp_fat);
        fclose(fp_arc);
    }
    else
    {
        if (fp_fat)
            fclose(fp_fat);

        if (fp_arc)
            fclose(fp_arc);

        printf("Failed to create archive files\n");
        return false;
    }


    return true;
}


// ------------------------------------------------------------------------
// Compress file data.
// ------------------------------------------------------------------------
int CompressData(const Bytef *data, uLong dataSize, uLong &dataOutSize, u8 **dataOut)
{
    if (dataOut == NULL)
        return COMPRESS_FAILED;

    // Generate buffer size and the output buffer.
    dataOutSize = compressBound(dataSize);
    *dataOut    = (u8*)malloc(dataOutSize);

    // Alloc succeeded?
    if (!*dataOut)
    {
        return COMPRESS_FAILED;
    }

    // Compress
    memset(*dataOut, 0, dataOutSize);
    int err = compress(*dataOut, &dataOutSize, data, dataSize);
    if (err == Z_OK)
    {
        if (dataOutSize >= dataSize)
        {
            free(*dataOut);
            *dataOut = NULL;
            return COMPRESS_LARGER;
        }

        return COMPRESS_SUCCESS;
    }
    else
    {
        free(*dataOut);
        *dataOut = NULL;

        if (err == Z_MEM_ERROR)
        {
            printf("Compression failed: Z_MEM_ERROR\n");
        }
        else if (err == Z_BUF_ERROR)
        {
            printf("Compression failed: Z_BUF_ERROR\n");
        }
        else
        {
            printf("Compression failed: Unknown error\n");
        }
    }

    return COMPRESS_FAILED;
}


// ------------------------------------------------------------------------
// Changes every occurrence of the search character with the replacement
// character.
// ------------------------------------------------------------------------
void StringReplaceChar(char *string, char search, char replace)
{
    if (string && *string)
    {
        while(*string)
        {
            if (*string == search)
                *string = replace;

            string++;
        }
    }
}


#ifdef EXAMPLE_CODE
ArcEntry * Locate(u32 hash, u32 entryCount, ArcEntry *pEntries);

// ------------------------------------------------------------------------
// Looks for some test files
// ------------------------------------------------------------------------
void Find()
{
    FILE *fp = fopen("arc_00.fat", "rb");
    if (fp)
    {
        // Get file size
        fseek(fp, 0, SEEK_END);
        s32 filesize = (s32)ftell(fp);
        rewind(fp);       

        // Read data
        u8 *buffer = (u8*)malloc(filesize);
        fread(buffer, 1, filesize, fp);                        
        fclose(fp);

        // Point to data
        FatHeader *pHeader     = (FatHeader*)(buffer);
        ArcEntry  *pArcEntries = (ArcEntry*)(buffer + sizeof(FatHeader));

        u32 count = pHeader->entries;
        printf("Entries: %i\n", count);

        ArcEntry *pEntry1 = Locate(StringHash("abc/a.txt"), count, pArcEntries);
        ArcEntry *pEntry2 = Locate(StringHash("textures/dialog.png"), count, pArcEntries);
        ArcEntry *pEntry3 = Locate(StringHash("textures/logo.png"), count, pArcEntries);

        if (pEntry1)
        {
            printf("Found '%s'\n", pEntry1->filename);
        }
        if (pEntry2)
        {
            printf("Found '%s'\n", pEntry2->filename);
        }
        if (pEntry3)
        {
            printf("Found '%s'\n", pEntry3->filename);
        }

        free(buffer);
    }
}


// ------------------------------------------------------------------------
// This function looks in an archive for a file.
// ------------------------------------------------------------------------
ArcEntry *Locate(u32 hash, u32 entryCount, ArcEntry *pEntries)
{
    ArcEntry *result = nullptr;

    for (u32 i=0; i<entryCount; i++)
    {
        s32  lower = 0;
        s32  upper = entryCount;

        // Get table start
        ArcEntry *pStart = pEntries;

        while(lower <= upper)
        {
            s32 mid = (lower + upper) / 2;

            // Get mid entry
            ArcEntry *pEntry = pStart;
            pEntry += mid;

            if (hash > pEntry->hash)
            {
                lower = mid + 1;
            }
            else if (hash < pEntry->hash)
            {
                upper = mid - 1;
            }
            else
            {
                result = pEntry;
                break;
            }
        }
    }

    return result;
}
#endif