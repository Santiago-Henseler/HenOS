#include "process.h"

processList procs;

void initProcess(){
    procs.size = 0;
}

void newProcess(void * codeSegment){
    
    if( procs.size >= 10)
        return;

    process new = {procs.size, NULL, NULL, codeSegment, NULL};

    memCopy((void *)&new, (void *)&procs.proc[procs.size], sizeof(process)); 

    procs.size++;

    runProcess(procs.proc[procs.size-1].codeSegment);
}
