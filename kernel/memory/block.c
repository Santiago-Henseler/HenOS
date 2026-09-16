#include "block.h"

extern uint8 kernelEnd;

memBlock * rootBlock = NULL;
memBlock * rootUsedBlock = NULL;

// Utilizo la primera seccion de memoria para guardar los structs
// organizados en 2 listas discriminando libres y usados
// para cuando se haga free del bloque chequear que el puntero dado sea correcto 
void initMemBlock(uint32 freeMemSize){

    uint32 blocks = freeMemSize / PAGE_SIZE;

    uint8 * kernelEndAddr = &kernelEnd;
    uint32  blockLen = (blocks * sizeof(memBlock));
    uint8 * blockAddr = ALIGN(kernelEndAddr + blockLen, PAGE_SIZE);

    memBlock * block = (memBlock *) kernelEndAddr;

    block[0].next = &block[1];
    block[0].addr = blockAddr;

    for(int i = 1; i < blocks-1; i++){ 
        block[i].addr = blockAddr + i*(PAGE_SIZE);
        block[i].next = &block[i+1];
    }

    block[blocks-1].addr = blockAddr+(blocks-1)*PAGE_SIZE;
    block[blocks-1].next = NULL;

    rootBlock = &block[0];
}

void * getRamBlock(){

    if(!rootBlock) // No hay mas memoria disponible
        return NULL;

    memBlock * block = rootBlock;

    rootBlock = rootBlock->next;
    block->next = rootUsedBlock; 
    rootUsedBlock = block;

    return block->addr;
}

void freeBlock(void * addr){
    if(!addr)
        return;

    memBlock * prev = NULL;
    memBlock * act = rootUsedBlock;
    bool found = false;
    
    while(act != NULL && !found){
        if(act->addr == addr){
            if(prev != NULL)
                prev->next = act->next;
            else
                rootUsedBlock = act->next;
            act->next = rootBlock;
            rootBlock = act;
            found = true;
        }   
        prev = act;
        act = act->next;
    }
}