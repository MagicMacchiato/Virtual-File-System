#include"vdisk.h"


/*--VDisk signature--*/
static const byte VDISK_SIG[VDISK_SIG_BYTE_SIZE] = {0x44, 0x6b, 0x49, 0x73, 0x53, 0x69, 0x4b, 0x64};

/*--Main functionalities--*/

FILE* mount_disk(const char* disk_name) {
    // open a disk(file).
    FILE* disk_ptr = fopen(disk_name, "rb+");

    // check if the disk can be open
    if(disk_ptr == NULL) {
        printf("Error opening a disk\n");
        return NULL;
    }

    // check if the disk is a valid disk
    if(!is_valid_disk(disk_ptr)) {
        printf("The disk is not valid.\n");
        fclose(disk_ptr);
        return NULL;
    }

    printf("Successfully mount a disk '%s'\n", disk_name);
    return disk_ptr;
}


int is_valid_disk(FILE* disk) {
    // read the first 8 bytes to the buffer
    byte sig_bytes[VDISK_SIG_BYTE_SIZE];

    fread(sig_bytes, sizeof(byte), VDISK_SIG_BYTE_SIZE, disk);

    // set the file pointer back to the start
    fseek(disk, 0, SEEK_SET);

    return memcmp(sig_bytes, VDISK_SIG, VDISK_SIG_BYTE_SIZE) == 0;
}


int create_disk(const char* file_name) {

    // check file validity
    if(strlen(file_name) <= 0) {
        printf("file_name is not valid (0 character).\n");
        return 1;
    }
    else if(strcmp(file_name, "\\") == 0 || strcmp(file_name, "/") == 0) {
        printf("file_name is not valid (only contains '\\' or '/').\n");
        return 1;
    }

    // check for file extension
    // file extension temp used for compare(size = 4(.cvd) + 1(Null terminator))
    char file_ext_temp[5];
    int offset = strlen(file_name) - VDISK_FILE_EXT_LEN + 1;
    strncpy(file_ext_temp, file_name + offset, VDISK_FILE_EXT_LEN);

    char* usable_file_name;

    if(strcmp(file_ext_temp, VDISK_FILE_EXTENSION) != 0) {
        usable_file_name = (char*)malloc(sizeof(char) * (strlen(file_name) + VDISK_FILE_EXT_LEN + 1)); // +1 for null terminator
        strcpy(usable_file_name, file_name);
        strcat(usable_file_name, VDISK_FILE_EXTENSION);
    }
    else {
        usable_file_name = (char*)malloc(sizeof(char) * (strlen(file_name) + 1)); // +1 for null terminator
        strcpy(usable_file_name, file_name);
    }

    // open the file writer
    FILE* disk_writer = fopen(usable_file_name, "wb");

    // check if disk_writer can be opened
    if(disk_writer == NULL) {
        printf("Error using disk_writer.\n");
        free(usable_file_name);
        return 1;
    }

    
    // assign a signature to the disk header
    fwrite(VDISK_SIG, sizeof(byte), VDISK_SIG_BYTE_SIZE, disk_writer);

    // assign creation date to the disk header
    char date_now[VDISK_DATE_BYTE_SIZE + 1]; // + 1 for null terminator
    make_date_time_now(date_now);
    fwrite(date_now, sizeof(char), VDISK_DATE_BYTE_SIZE, disk_writer);


    // assign 0 to the used space amount header
    unsigned int used_space = 0;
    fwrite(&used_space, sizeof(unsigned int), 1, disk_writer);

    // assign max disk size to the left space amount header
    unsigned int left_space = (unsigned int)VDISK_MAX_SIZE;
    fwrite(&left_space, sizeof(unsigned int), 1, disk_writer);

    // assign name to the disk header

    char name[NAME_MAX_LENGTH];

    // find '/' or '\' and get its location (backward)
    int stop_at = -1;

    for(int i = strlen(usable_file_name); i >= 0;i--) {
        if(usable_file_name[i] == '/' || usable_file_name[i] == '\\') {
            stop_at = i;
            break;
        }
    }

    if(stop_at != -1) {
        stop_at++; // move the pointer to the actual character.

        // make a substring
        strncpy(name, usable_file_name + stop_at, strlen(usable_file_name) - stop_at - VDISK_FILE_EXT_LEN);
    }
    else {
        strncpy(name, usable_file_name, strlen(usable_file_name) - VDISK_FILE_EXT_LEN);
    }

    fwrite(name, sizeof(char), NAME_MAX_LENGTH, disk_writer);



    // set all inodes to 0

    //move to the second block
    fseek(disk_writer, BLOCK_SIZE * INODE_START_BLOCK, SEEK_SET);

    // write 1 block per time
    long written = 0;
    byte* zero_buffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);
    memset(zero_buffer, (byte)0, BLOCK_SIZE);

    for(int i = 0;i < INIT_INODE_BLOCK_AMOUNT;i++){
        written += fwrite(zero_buffer, sizeof(byte), BLOCK_SIZE, disk_writer);
    }

    if(written < INIT_INODE_BYTE_AMOUNT) {
        printf("Inode is not fully written, only wrote %ld.\n", written);
    }

    // free the buffer
    free(zero_buffer);
    

    // close the file after done
    printf("Successfully created a disk name '%s'.\n", name);

    free(usable_file_name);
    fclose(disk_writer);

    return 0;
}


DiskProperty get_disk_metadata(FILE* disk) {

    // for returning in case of invalid case
    DiskProperty invalid_prop;
    strcpy(invalid_prop.name, "$!INVALID_DISK$!");
    strcpy(invalid_prop.creation_date_time, "NA");
    invalid_prop.space_left = -1;
    invalid_prop.used_space = -1;

    if(!is_valid_disk(disk)) {
        printf("The disk is not valid");
        return invalid_prop;
    }

    DiskProperty disk_prop;

    // extract a disk name
    fseek(disk, VDISK_NAME_START_BYTE, SEEK_SET);

    char name_temp[NAME_MAX_LENGTH];
    fread(name_temp, sizeof(char), NAME_MAX_LENGTH, disk);

    strcpy(disk_prop.name, name_temp);

    // extract a date
    fseek(disk, VDISK_DATE_START_BYTE, SEEK_SET);

    char date_temp[VDISK_DATE_BYTE_SIZE];
    fread(date_temp, sizeof(char), VDISK_DATE_BYTE_SIZE, disk);

    strcpy(disk_prop.creation_date_time, date_temp);

    // extract used space
    fseek(disk, VDISK_USED_START_BYTE, SEEK_SET);
    unsigned int used_temp;
    fread(&used_temp, sizeof(unsigned int), 1, disk);

    disk_prop.used_space = used_temp;

    // extract space left
    fseek(disk, VDISK_LEFT_START_BYTE, SEEK_SET);
    unsigned int left_temp;
    fread(&left_temp, sizeof(unsigned int), 1, disk);

    disk_prop.space_left = left_temp;

    return disk_prop;
}


/*--Helper functions--*/

int make_date_time_now(char* buffer) {
    time_t now = time(NULL);
    struct tm* local_now = localtime(&now);

    // ddmmyyyy(8 bytes) + hhmmss(6 bytes) + 1(Null terminator)

    strftime(buffer, 15, "%d%m%Y%H%M%S", local_now);

    return 0;
}




