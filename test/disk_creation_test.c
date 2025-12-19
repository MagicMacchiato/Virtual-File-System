#include"../include/vdisk.h"


int main(void) {
    char* cases[7] = {"", "case1", "case2.cvd", "case3.ccc", "case4.cc", "/test2/case5", "/"};

    for(int i = 0;i < 6;i++) {
        create_disk(cases[i]);
    }

    return 0;
}