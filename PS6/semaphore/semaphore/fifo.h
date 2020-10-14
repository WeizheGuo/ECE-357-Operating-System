//
//  fifo.h
//  semaphore
//
//  Created by GUOWEIZHE on 12/25/18.
//  Copyright © 2018 GUOWEIZHE. All rights reserved.
//

#ifndef fifo_h
#define fifo_h

#include <stdio.h>

#define MYFIFO_BUFSIZ 4000

struct Fifo {
    struct Sem s_r, s_w, s_rk, s_wk;
    int *start, *end;
    unsigned long *queue;
};

void fifo_init(struct Fifo *f);
void fifo_wr(struct Fifo *f,unsigned long d);
unsigned long fifo_rd(struct Fifo *f);
void fifo_destroy(struct Fifo *f);

#endif /* fifo_h */
