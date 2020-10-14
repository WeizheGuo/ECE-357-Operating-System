//
//  sem.h
//  semaphore
//
//  Created by GUOWEIZHE on 12/24/18.
//  Copyright © 2018 GUOWEIZHE. All rights reserved.
//

#ifndef sem_h
#define sem_h
#define N_PROC 64

#include <stdio.h>

#define N_PROC 64

struct Sem {
    char *lock;
    int *count;
    char *process_sleeping;
    int *process_id;
};

void Sem_init(struct Sem *s, int count);
int Sem_try(struct Sem *s);
void Sem_wait(struct Sem *s);
void Sem_inc(struct Sem *s);
void Sem_destroy(struct Sem *s);

#endif /* sem_h */
