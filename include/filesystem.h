#pragma once

#include"libs.h"
#include"vdisk.h"

#ifndef FILE_SYSTEM_H

/*--Define file system--*/
#define FILE_SYSTEM_H

/*--Declare field--*/

#define WRITABLE_START_BLOCK 32
#define READ_DISK_FILE_MODE "rb+"

// if cur_byte_ptr == next_byte_ptr means there is no next block referring to.
typedef struct FileSystemObject {
    char name[FILENAME_MAX];
    char creation_date[9]; // 8 for the disk date format + 1(Null terminator)
    byte is_dir; // contains whether 0 or 1 *only*
    unsigned long size;
    unsigned long cur_byte_ptr;
    unsigned long next_byte_ptr;
}FObj;



#endif