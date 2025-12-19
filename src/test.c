#include"../include/vdisk.h"



int main(void) {

    int status = create_disk("my_disk");

    if(status != 0) {
        return 1;
    }
    else {
        FILE* disk = mount_disk("my_disk.cvd");

        if(disk != NULL) {
            DiskProperty prop = get_disk_metadata(disk);

            printf("name: %s\ndate-time: %s\nused space: %u\nspace left: %u\n", prop.name, prop.creation_date_time, prop.used_space, prop.space_left);
            fclose(disk);
        }
        else {
            return 1;
        }
    }

    return 0;
}