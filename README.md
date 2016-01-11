# ProteusArchiveTool

This tool archives and optionally compresses the contents of a directory into two files. One file is all the data, the second file is the file allocation table. The file allocation table contains all the required data to locate a file within the archive

This is designed to hide files from basic users to prevent modding of files. It does not encrypt files, it merely makes files harder to change.

Files are alphabetically sorted and use a numerical hash which is designed to allow simple search binary implementations.