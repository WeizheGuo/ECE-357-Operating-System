//
//  fifo.c
//  semaphore
//
//  Created by GUOWEIZHE on 12/25/18.
//  Copyright © 2018 GUOWEIZHE. All rights reserved.
//

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "sem.h"
#include "fifo.h"

void fifo_init(struct Fifo *f) {
    Sem_init(&f->s_r, 0);
    Sem_init(&f->s_w, MYFIFO_BUFSIZ);
    Sem_init(&f->s_wk, 1);
    Sem_init(&f->s_rk, 1);
    void *map = mmap(NULL, sizeof(int) * 2 + MYFIFO_BUFSIZ * sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (map == MAP_FAILED) {
        fprintf(stderr, "Error while creating shared map: %s\n", strerror(errno));
        exit(-1);
    }
    f->start = map;
    *f->start = 0;
    map += sizeof(int);
    f->end = map;
    *f->end = 0;
    map += sizeof(int);
    f->queue = map;
}

void fifo_wr(struct Fifo *f, unsigned long d) {
    Sem_wait(&f->s_wk);
    Sem_wait(&f->s_w);
    f->queue[*f->end % MYFIFO_BUFSIZ] = d;
    (*f->end)++;
    Sem_inc(&f->s_wk);
    Sem_inc(&f->s_r);
}

unsigned long fifo_rd(struct Fifo *f) {
    Sem_wait(&f->s_rk);
    Sem_wait(&f->s_r);
    unsigned long result = f->queue[*(f->start) % MYFIFO_BUFSIZ];
    (*f->start)++;
    Sem_inc(&f->s_rk);
    Sem_inc(&f->s_w);
    return result;
}

void fifo_destroy(struct Fifo *f) {
    Sem_destroy(&f->s_r);
    Sem_destroy(&f->s_w);
    Sem_destroy(&f->s_wk);
    Sem_destroy(&f->s_rk);
    munmap(f->start, sizeof(int) * 2 + MYFIFO_BUFSIZ * sizeof(long));
}
