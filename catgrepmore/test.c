#include <unistd.h>
#include <stdio.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <errno.h>

int signo,nsig,counter;
int rcvr_pid;

void handler(s){
    if (s==signo){
       counter++;
    }
}

int main(ac,av)
char **av;
{
    struct sigaction sa;
    int pid,status,nchild,i;
        signo=SIGUSR1;
        nsig=10000;
        rcvr_pid=getpid();
        sa.sa_handler=handler; /* WHAT would happen if I forgot this line below? */
        sa.sa_flags=0;
        sigemptyset(&sa.sa_mask);
        if (sigaction(signo,&sa,0)== -1) {
            perror("sigaction"); return -1;
            
        }
        switch(pid=fork()) {
            case -1:
                perror("fork");
                return -1;
            case 0:
                sender();
                return 0;
            
        }
        fprintf(stderr,"Waiting for sender\n");
        while (wait(&status)>0|| errno==EINTR)
            ;
        printf("stderr,Received a total of %d of signal #%d\n",counter,signo);
    
}

sender() {
    int i;
    
    for(i=0;i<nsig;i++) {
        kill(rcvr_pid,signo);
    }
    
    
}
