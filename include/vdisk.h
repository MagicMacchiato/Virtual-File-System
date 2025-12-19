#pragma once

#include"libs.h"

#ifndef V_DISK_H

/*--Define v disk--*/
#define V_DISK_H

typedef char byte;

/*--Declare field--*/

// File extension for virtual disk used in this project
#define VDISK_FILE_EXTENSION ".cvd" // (.cvd) file (C Virtual Disk)
#define VDISK_FILE_EXT_LEN 4

// Declare const for virtual disk
#define VDISK_MAX_SIZE 4000000000 // The maximum disk capacity (Default: 4GB)
#define BLOCK_SIZE 4000 // The block size in a disk (Default: 4KB)

#define READ_WRITE_BLOCKS_LIMIT 10 // limit for reading and writing blocks from a disk per one call

#define INIT_INODE_BYTE_AMOUNT 124000
#define INIT_INODE_BLOCK_AMOUNT 31

// Reserved blocks for the disk (SUM 32 blocks reserved for the disk)

enum ReservedBlocks{
    DISK_HEADER_BLOCK = 0,
    INODE_START_BLOCK = 1,
    INODE_END_BLOCK = 31
};

// Disk header structure(in byte)

#define VDISK_SIG_BYTE_SIZE 8
#define VDISK_DATE_BYTE_SIZE 15
#define VDISK_USED_BYTE_SIZE 4
#define VDISK_LEFT_BYTE_SIZE 4
#define VDISK_NAME_BYTE_SIZE 256

enum DiskHeader{
    VDISK_SIG_START_BYTE = 0,
    VDISK_SIG_END_BYTE = 7,
    
    // Contains in ddmmyyyy format (E.g. 16122025 # 16 December 2025)
    VDISK_DATE_START_BYTE = 8,
    VDISK_DATE_END_BYTE = 22,

    VDISK_USED_START_BYTE = 23,
    VDISK_USED_END_BYTE = 26,

    VDISK_LEFT_START_BYTE = 27,
    VDISK_LEFT_END_BYTE = 30,
    
    VDISK_NAME_START_BYTE = 31,
    VDISK_NAME_END_BYTE = 286
};

#define NAME_MAX_LENGTH 256
#define DATE_TIME_MAX_LENGTH 15

typedef struct DiskProperty{
    char name[NAME_MAX_LENGTH];
    char creation_date_time[DATE_TIME_MAX_LENGTH]; // 15 for the disk date format + 1(Null terminator)
    unsigned int space_left;
    unsigned int used_space;

}DiskProperty;

/*--Functions declare--*/

/*
A function that will mount the program to a disk.

Params:
disk_name: a disk name.

Return:
file pointer: success
NULL: failed
*/
FILE* mount_disk(const char* disk_name);


/*
A function that check if the passed disk is a valid disk or not.

Params:
disk: a file pointer to a virtual disk(in rb+ mode).

Return:
1: is a valid disk(true value)
0: is not a valid disk(false value)
*/
int is_valid_disk(FILE* disk);


/*
A function that creates a new virtual disk.

Params:
disk_name: a disk name.

Return:
0: success
1: failed
*/
int create_disk(const char* disk_name);



/*
TODO: Make it produce hours and minutes as well (full format ddmmyyyyhhmm)
A function that create a time in format ddmmyyyy. Specified for the virtual disk usage.

Params:
buffer: a buffer to hold the time format in (ddmmyyyyhhmmss)

Return:
0: success
*/
int make_date_time_now(char* buffer);


/*
A function that extract a disk metadata

Params:
disk: a file pointer to a virtual disk(in rb+ mode).

Return:
a structure of a disk property(DiskProperty).
*/
DiskProperty get_disk_metadata(FILE* disk);


/*
A function that will lookup inode bit map and reserve vacant memory blocks.

Params:
amount: the amount of block that want to reserve.

Return (Malloc):
An array of unsigned long, represents vacant and ready to be written blocks.
*/
unsigned long* reserve_blocks(const unsigned long amount);


/*
A function that write byte stream to blocks in the disk.

Params:
disk: A file pointer to a virtual disk.
stream: A stream to write.
blocks: Blocks to write.

Return:
0: write success
1: write failed
*/
int write_to(FILE* disk, byte* stream, const unsigned long* blocks);

/*
A function that read byte stream from blocks of the disk.

Params:
disk: A file pointer to a virtual disk.
blocks: Blocks to read.

Return (Malloc):
An array of byte stream

*/
byte* read_from(FILE* disk, const unsigned long* blocks);

#endif