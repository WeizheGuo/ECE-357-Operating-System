//
//  main.c
//  finddir
//
//  Created by GUOWEIZHE on 9/30/18.
//  Copyright © 2018 GUOWEIZHE. All rights reserved.
//

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define FAILURE -1

int recursive_find(DIR *cur_dir, char *path);
char* concat(const char *s1, const char *s2);
int print_info(char *cur_path);

int main(int argc, char **argv){
    char* dir = "./";

    if(argc == 2){
       dir = argv[1];
    }
    else if(argc > 2){
        fprintf(stderr, "Error: Wrong input format\n");
        return -1;
    }
    
    DIR *start_dir = opendir(dir);
    if(start_dir == NULL){
        fprintf(stderr, "Error: cannot open directory %s: %s\n", dir, strerror(errno));
        return -1;
    }
    else{
        if(print_info(dir) == -1)
            return -1;
        // recursive call function to open dir
        if(recursive_find(start_dir, dir) == -1)
            return -1;
        }
    if (closedir(start_dir) == -1) {
        fprintf(stderr, "Error: cannot close directory: %s: %s\n", dir, strerror(errno) );
        return -1;
    }
    return 0;
}

int recursive_find(DIR *cur_dir, char *path){
    struct dirent *dp;
    char *cur_path = ".";
    
    while((dp = readdir(cur_dir)) != NULL){
        //strncpy(cur_path, path, sizeof(cur_path));
        
        if((strcmp(dp->d_name, "..") == 0)||(strcmp(dp->d_name, ".") == 0))
            continue;
        if(strcmp(dp->d_name, ".") != 0)
            cur_path = concat(concat(path, "/"), dp->d_name);
        if ((print_info(cur_path) == -1))
            return -1;
        if(dp->d_type == DT_DIR && strcmp(dp->d_name, ".") != 0){
            DIR *new_dir = opendir(cur_path);
            if(new_dir == NULL){
                fprintf(stderr, "Error: cannot open directory %s: %s\n", cur_path, strerror(errno));
                return -1;
            }
            if(recursive_find(new_dir, cur_path) == -1)
                return -1;
            if (closedir(new_dir) == -1){
                fprintf(stderr, "Error: cannot close directory %s: %s\n", cur_path, strerror(errno));
                return -1;
            }
        }
    }
    return 0;
}

int print_info(char *cur_path){
    // Print info about all files in the directory
    // Acknowledgement: Example from https://linux.die.net/man/2/lstat
    struct stat stat;
    if(lstat(cur_path, &stat) == -1){
        fprintf(stderr, "Error: cannot get status of file %s: %s\n", cur_path, strerror(errno));
        return -1;
    }
    
    char mode[11];
    switch (stat.st_mode & S_IFMT) {
        case S_IFBLK:  mode[0] = 'b';        break;
        case S_IFCHR:  mode[0] = 'c';        break;
        case S_IFDIR:  mode[0] = 'd';        break;
        case S_IFIFO:  mode[0] = 'p';        break;
        case S_IFLNK:  mode[0] = 'l';        break;
        case S_IFREG:  mode[0] = '-';        break;
        case S_IFSOCK: mode[0] = 's';        break;
        default:       mode[0] = '?';        break;
    }
    // using ternary operator '?' to simplify the code
    mode[1] = (stat.st_mode & S_IRUSR) ? 'r' : '-';
    mode[2] = (stat.st_mode & S_IWUSR) ? 'w' : '-';
    mode[3] = (stat.st_mode & S_IXUSR) ? 'x' : '-';
    mode[4] = (stat.st_mode & S_IRGRP) ? 'r' : '-';
    mode[5] = (stat.st_mode & S_IWGRP) ? 'w' : '-';
    mode[6] = (stat.st_mode & S_IXGRP) ? 'x' : '-';
    mode[7] = (stat.st_mode & S_IROTH) ? 'r' : '-';
    mode[8] = (stat.st_mode & S_IWOTH) ? 'w' : '-';
    mode[9] = (stat.st_mode & S_IXGRP) ? 'x' : '-';
    mode[10] = 0;
    
    /* printing user id and group id credit to https://stackoverflow.com/questions/50091777/printing-information-from-stat2*/
    struct passwd *pwd;
    struct group *grp;
    char userid[256], groupid[256];
    pwd = getpwuid(stat.st_uid);
    grp = getgrgid(stat.st_gid);
    if((pwd == NULL)||(grp == NULL)){
        fprintf(stderr, "Error: cannot print file user id or group id: %s\n", strerror(errno));
        return -1;
    }
    else{
        strcpy(userid, pwd->pw_name);
        strcpy(groupid, grp->gr_name);
    }
    
    char mtime[60];
    time_t now;
    now = time(NULL);
    
    if(localtime(&stat.st_mtime)->tm_year == localtime(&now)->tm_year){
        strftime(mtime, sizeof(mtime), "%b %e %R", localtime(&stat.st_mtime));
    }
    else{
        strftime(mtime, sizeof(mtime), "%b %e  %Y", localtime(&stat.st_mtime));
    }
    
    if (mode[0] == 'l') {
        char linkfile[256];
        readlink(cur_path, linkfile, sizeof(linkfile));
        cur_path = concat(concat(cur_path, " -> "), linkfile);
    }
    printf("%llu %8lld %s %4hu %s %12s %19lld %s %s\n", stat.st_ino, stat.st_blocks,
           mode, stat.st_nlink, userid, groupid, stat.st_size, mtime, cur_path);
    return 0;
}

char* concat(const char *s1, const char *s2){
    char *result = malloc(strlen(s1)+strlen(s2)+1);
    if (result == NULL) {
        fprintf(stderr, "Error: cannot concatenate strings %s and %s: %s\n", s1, s2,strerror(errno));
        exit(EXIT_FAILURE);
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

