#include "page.h"

// TODO: deberia tener un page table por proceso

extern uint8 kernelEnd;
extern uint8 kernelStart;

extern void setPageDirectory(pageDirectoryEntry* pd);
extern void startPagination();

int mapPage(void *pa, void *va, uint32 flags);

pageDirectoryEntry * pageDirectory = PAGE_FLAG_NO_PRESENT;  

void initPageTable(uint32 freeMemSize){
    
    void * ramBlock = getRamBlock();
    if(ramBlock == NULL)
        kernelPanic();

    pageDirectory = ALIGN(ramBlock, PAGE_SIZE); 

    for(int i = 0; i < PAGE_DIR_SIZE; i++){
        pageDirectory[i] = PAGE_FLAG_NO_PRESENT; 
    }

    uint32 blocks = freeMemSize / PAGE_SIZE;
    for (uint32 addr = 0; addr < ALIGN(((uint32)&kernelEnd  + blocks*sizeof(memBlock)), PAGE_SIZE); addr += PAGE_SIZE) {
        mapPage((void*)addr, (void*)addr, PAGE_FLAG_KERNEL | PAGE_FLAG_WRITE | PAGE_FLAG_PRESENT);
    }
    
    mapPage(MEM_VGA, MEM_VGA, PAGE_FLAG_KERNEL | PAGE_FLAG_WRITE | PAGE_FLAG_PRESENT);

    mapPage(pageDirectory, pageDirectory, PAGE_FLAG_WRITE  | PAGE_FLAG_PRESENT| PAGE_FLAG_KERNEL);

    setPageDirectory(pageDirectory);
    startPagination();
}
 
pageTableEntry * newPageTableEntry(){

    void * ramBlock = getRamBlock();
    if(ramBlock == NULL)
        return NULL;

    pageTableEntry * pageTable = (pageTableEntry *)ALIGN(ramBlock, 4096);

    for(int i = 0; i < PAGE_TABLE_SIZE; i++){
        pageTable[i] = PAGE_FLAG_NO_PRESENT; 
    }

    return pageTable;
}  

int mapPage(void *pa, void *va, uint32 flags){
    uint32 pageDirPos = ((uint32)va & VA_TO_PD_MASK) >> 22;
    uint32 pageTablePos = ((uint32)va & VA_TO_PT_MASK) >> 12;

    if (!(pageDirectory[pageDirPos] & PAGE_FLAG_PRESENT)) {
        pageTableEntry *newP = newPageTableEntry();
        if(newP == NULL)
            return -1;
        pageDirectory[pageDirPos] = ((uint32)newP & PDE_TO_PT_MASK) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
        if(mapPage(newP, newP, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE ) == -1)
            return -1;
    }

    pageTableEntry *pageTable = (pageTableEntry *)(pageDirectory[pageDirPos] & PDE_TO_PT_MASK);

    if (pageTable[pageTablePos] & PAGE_FLAG_PRESENT)
        return -1;

    pageTable[pageTablePos] = ((uint32)pa & PA_TO_ADDR_MASK) | flags;
    
    return 0;
}

void * getPage(){

    void * addr = ALIGN(getRamBlock(), PAGE_SIZE);

    if(addr == NULL)
        return NULL;

    mapPage(addr, addr, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE | PAGE_FLAG_WT);

    return addr;
}

void freePage(void * va){
    if(va == NULL)
        return;

    uint32 pageDirPos = ((uint32)va & VA_TO_PD_MASK) >> 22;
    uint32 pageTablePos = ((uint32)va & VA_TO_PT_MASK) >> 12;

    pageTableEntry *pageTable = (pageTableEntry *)(pageDirectory[pageDirPos] & PDE_TO_PT_MASK);

    uint32 pa =  pageTable[pageTablePos];
    uint32 addr = (pa & PA_TO_ADDR_MASK) |  (uint32)va & VA_TO_OFFSET;

    pageTable[pageTablePos] = PAGE_FLAG_NO_PRESENT;

    freeBlock(addr);
}