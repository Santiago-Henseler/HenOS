#include "sched.h"

extern process * actualProcess;
extern processList * procs;

void sched(){

    uint8 next = 0;

    if(actualProcess != NULL){
        actualProcess->state = READY;
        next = actualProcess->pid + 1 % procs->size;
    }else if(actualProcess == NULL && procs->size > 0){
        next =  procs->size-1;
    }else{
        yield();
    }

    actualProcess = procs->proc[next];

    execProcess(next);
}

void yield(){
    printf("No hay mas procesos a mimir");
    for(;;);
}