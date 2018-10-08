//  main.c
//  minicat
//
//  Created by GUOWEIZHE on 9/16/18.
//  Copyright © 2018 GUOWEIZHE. All rights reserved.
//

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

extern char *optarg;
extern int opting;

int main(int argc, const char * argv[]) {
    int opt=0;
    int readsize, infd;
    int outfd = STDOUT_FILENO;
    int buffersize = 512;
    char *buffer;
    char *outfile = NULL;
    
    while ((opt = getopt(argc, argv, "b:o:"))!= -1){
        switch(opt){
                case 'b':
                        buffersize = atoi(optarg);
                        break;
                case 'o':
                        outfile = optarg;
                        break;
                case '?':
                        fprintf(stderr,"Error: Invalid option character\n");
                        return -1;
                        break;
                default:
                        break;
       
        }
    }
    
    if(buffersize < 0){
        fprintf(stderr,"Error: Invalid buffersize\n");
        return -1;
    }
    
    buffer = malloc(buffersize);
    
    if(outfile != NULL){
        outfd = open(outfile, O_RDWR|O_CREAT|O_TRUNC,0666);
        if(outfd < 0){
            fprintf(stderr,"Error when opening output file named %s: %s\n", outfile, strerror(errno));
            return -1;
        }
    }
    // The following block solves cases that has no input arguement
     if(optind == argc){
        optind --;
        argv[optind] = "-";
    }
    
    for (int i = optind; i <argc; i++){
        infd = STDIN_FILENO;
        if(strcmp(argv[i],"-")!=0){
            infd = open (argv[i], O_RDONLY);
            if (infd < 0){
                fprintf(stderr, "Error when opening the input file named %s: %s\n", argv[i], strerror(errno));
                return -1;
            }
            
        }
        
        // read each file until it reaches the end, where readsize would equal to 0
        do{
            readsize = read(infd, buffer, buffersize);
            if(readsize < 0){
                fprintf (stderr, "Error when reading from the input file named %s: %s\n", argv[i], strerror(errno));
                return -1;
            }
            else {
                // handling partial write by the following loop
                /* Reference about dealing with partial write: https://stackoverflow.com/questions/694188/when-does-the-write-system-call-write-all-of-the-requested-buffer-versus-just */
                int writesize, size;
                char * wbuffer;
                wbuffer = buffer;
                size = readsize;
                while (((writesize = write(outfd, wbuffer, size)) != size) && size > 0) {
                    if(writesize < 0 && errno == EINTR)
                        continue;
                    if(writesize < 0) {
                        fprintf(stderr, "Error when writing to the output file named %s: %s\n", outfile, strerror(errno));
                        return -1;
                    }
                    size -= writesize;
                    buffer += writesize;
                }
            }
        }
        while(readsize != 0);
        
        if (infd != STDIN_FILENO){
            if(close(infd) < 0){
                fprintf(stderr, "Error when closing the input file named %s: %s\n", argv[i], strerror(errno));
                return -1;
            }
        }
    }
    
    if(close(outfd) < 0){
        fprintf(stderr, "Error when closing the output file named %s: %s\n", outfile, strerror(errno));
        return -1;
    }
    free(buffer);
    return 0;
}
