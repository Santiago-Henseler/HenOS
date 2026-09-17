#include "stdmem.h"

extern uint8 kernelEnd;

typedef struct allockBlock {
    void * addr;
    uint32 size;
    struct allockBlock * next;
    bool used;
}  allockBlock __attribute__((aligned(PAGE_SIZE)));

allockBlock * rootAllocBlock = NULL;

allockBlock * newAllockBlock(){
    allockBlock* new = (allockBlock *)page();
    if(new == NULL) 
        return NULL;

    new->size = PAGE_SIZE;
    new->addr = (void *)(new + 1);
    new->next = NULL;
    new->used = false;

    return new;
}

void splitAllocBlock(allockBlock * left) {
    allockBlock * right = (allockBlock *)((uint8 *)left + left->size / 2);

    right->size = left->size / 2;
    right->addr = (void *)(right + 1);
    right->used = false;
    right->next = left->next;

    left->size /= 2;
    left->next = right;
}

void joinAllocBlock(allockBlock * left){
    allockBlock * right = left->next;

    left->next = right->next;
    left->size += right->size;
}

// Devuelve un puntero al bloque libre 
void * getAllocBlock(uint32 size){
    uint32 realSize = ALIGN(size + sizeof(allockBlock), 16);

    if(rootAllocBlock == NULL)
        rootAllocBlock = newAllockBlock();

    allockBlock * candidate = NULL;
    allockBlock * act = rootAllocBlock;
    allockBlock * last = NULL;

    while(act != NULL){
        // de todos los bloques agarro el mas chiquito
        if (!act->used && act->size >= realSize) {
            if(!candidate)
                if (act->size < candidate->size)
                    candidate = act;
        }
        last = act;
        act = act->next;
    }
    if(candidate == NULL){
        // si se recorrio todos y no entraba, pido otro bloque
        allockBlock * new = newAllockBlock();
        if(new == NULL)
            return NULL;
        last->next = new;
        candidate = new;
    }
    
    while(candidate->size/2 >= realSize){
        // Del bloque mas chico que hay lo ajusto lo mejor posible al tamaño pedido
        splitAllocBlock(candidate);
    }

    candidate->used = true;
    return candidate->addr;
}

void memCopy(void * src, void * dst, uint32 size){

    uint8 * dstB = (uint8 *) dst;
    uint8 * srcB = (uint8 *) src;

    for(uint32 i = 0; i < size; i++)
        dstB[i] = srcB[i]; 

}

void memSet(void * dst, uint8 value, uint32 size){

    uint8 * dstB = (uint8 *) dst;

    for(uint32 i = 0; i < size; i++)
        dstB[i] = value; 
}

void * malloc(uint32 size){

    if(size > PAGE_SIZE){
        // TODO: Deberia poder reservar secciones de memoria mas grandes
        return NULL;
    }

    return getAllocBlock(size);
}

void * calloc(uint32 size){

    uint8 * addr = (uint8 *)malloc(size);

    if(addr == NULL)
        return NULL;

    for (uint32 i = 0; i < size; i++){
        uint8 * cAddr = addr +i;
        *cAddr = 0;
    }
    return addr;
}

void * realloc(void * ptr, uint32 size) {
    if (!ptr)
        return NULL;

    if(size == 0){
        free(ptr);
        return NULL;
    }

    allockBlock * old = ((allockBlock *)ptr) - 1;
    uint32 oldPayload = old->size - sizeof(allockBlock);

    if (oldPayload >= size)
        return ptr;

    void * newPtr = malloc(size);
    if (!newPtr)
        return NULL;

    uint32 newSize = oldPayload < size ? oldPayload : size;
    memCopy(ptr, newPtr, newSize);
    free(ptr);

    return newPtr;
}

void free(void * ptr) {
    if (!ptr)
        return;

    allockBlock * blk = ((allockBlock *)ptr) - 1;
    blk->used = false;

    allockBlock * act = rootAllocBlock;
    while (act && act->next) {
        allockBlock * next = act->next;

        bool sameSize = act->size == next->size;
        bool contiguous = ((uint8 *)act + act->size) == (uint8 *)next;

        if (!act->used && !next->used && sameSize && contiguous) {
            joinAllocBlock(act);
            continue;
        }

        act = act->next;
    }

    if(act->size == PAGE_SIZE)
        freeBlock(act);
}
