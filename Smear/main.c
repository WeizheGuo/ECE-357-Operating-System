//
//  main.c
//  Smear
//
//  Created by GUOWEIZHE on 11/28/18.
//  Copyright © 2018 GUOWEIZHE. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

int mapfile(const char *filename, const char *target, const char *replace);
void search_and_replace(void *address, off_t size, const char *target, const char *replace);

int main(int argc, const char * argv[]) {
    const char *target = argv[1];
    const char *replace = argv[2];
    
    if(argc < 4){
        fprintf(stderr, "Incorrect usage. Input format is smear TARGET REPLACEMENT file1 {file 2....}\n");
        return -1;
    }
    if(strlen(argv[1])!= strlen(argv[2])){
        fprintf(stderr, "Incorrect usage: TARGET and REPLACEMENT should be have equal length \n");
        return -1;
    }
    
    for(int i = 3; i < argc; i++){
        if(mapfile(argv[i], target, replace) == -1)
            return -1;
    }
    return 0;
}

int mapfile(const char *filename, const char *target, const char *replace){
    int fd;
    void *pa;
    if((fd = open(filename, O_RDWR)) < 0){
        fprintf(stderr, "Error: cannot open file named %s for reading and writing: %s\n", filename, strerror(errno));
        return -1;
    }
    struct stat st;
    fstat(fd, &st);
    off_t size = st.st_size;
    
    if ((pa = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
        fprintf(stderr, "Error: cannot map file named %s: %s\n", filename, strerror(errno));
        return -1;
    }
    search_and_replace(pa, size, target, replace);
    if (munmap(pa, size) == -1){
        fprintf(stderr, "Error: cannot unmap the file named %s: %s\n", filename, strerror(errno));
        return -1;
    }
    if (close(fd) == -1){
        fprintf(stderr, "Error: cannot close file named %s: %s\n", filename, strerror(errno));
        return -1;
    }
    return 0;
}

void search_and_replace(void *address, off_t size, const char *target, const char *replace){
    void *target_loc;
    while(1){
        target_loc = memmem(address, size, target, strlen(target));
        if(target_loc == NULL){
            // I am assuming we cannot find the target at any place in the file is not conisdered as an error
            break;
        }
        memcpy(target_loc, replace, strlen(replace));
        size -= address - target_loc;
        address = target_loc+1;
    }
}
