#include "page.h"

// TODO: deberia tener un directory por proceso

extern uint8 kernelEnd;
extern uint8 kernelStart;

extern void setPageDirectory(pageDirectoryEntry* pd);
extern void startPagination();
void mapPage(void *pa, void *va, uint32 flags);

pageDirectoryEntry * pageDirectory = PAGE_FLAG_NO_PRESENT;  

void initPageTable(uint32 freeMemSize){
    
    pageDirectory = ALIGN(getRamBlock(), PAGE_SIZE); 

    for(int i = 0; i < PAGE_DIR_SIZE; i++){
        pageDirectory[i] = PAGE_FLAG_NO_PRESENT; 
    }

    uint32 blocks = freeMemSize / PAGE_SIZE;
    for (uint32 addr = 0; addr < ALIGN(((uint32)&kernelEnd + blocks*sizeof(memBlock)), PAGE_SIZE); addr += PAGE_SIZE) {
        mapPage((void*)addr, (void*)addr, PAGE_FLAG_KERNEL  | PAGE_FLAG_WRITE | PAGE_FLAG_PRESENT);
    }

    mapPage(MEM_VGA, MEM_VGA, PAGE_FLAG_KERNEL | PAGE_FLAG_WRITE | PAGE_FLAG_PRESENT);

    mapPage(pageDirectory, pageDirectory, PAGE_FLAG_WRITE  | PAGE_FLAG_PRESENT| PAGE_FLAG_KERNEL);

    setPageDirectory(pageDirectory);
    startPagination();
}
 
pageTableEntry * newPageTable(){

    pageTableEntry * pageTable = (pageTableEntry *)ALIGN(getRamBlock(), 4096);

    for(int i = 0; i < PAGE_TABLE_SIZE; i++){
        pageTable[i] = PAGE_FLAG_NO_PRESENT; 
    }

    return pageTable;
}  

void mapPage(void *pa, void *va, uint32 flags){
    //TODO: agregar mejores chequeos
    uint32 pageDirPos = ((uint32)va & VA_TO_PD_MASK) >> 22;
    uint32 pageTablePos = ((uint32)va & VA_TO_PT_MASK) >> 12;

    if (!(pageDirectory[pageDirPos] & PAGE_FLAG_PRESENT)) {
        pageTableEntry *newP = newPageTable();
        pageDirectory[pageDirPos] = ((uint32)newP & PDE_TO_PT_MASK) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
        mapPage(newP, newP, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE );
    }

    pageTableEntry *pageTable = (pageTableEntry *)(pageDirectory[pageDirPos] & PDE_TO_PT_MASK);

    if (pageTable[pageTablePos] & PAGE_FLAG_PRESENT)
        return; // TODO: esta pagina fue ya mapeada

    pageTable[pageTablePos] = ((uint32)pa & PA_TO_ADDR_MASK) | flags;
}

void * getPage(){

    void * addr = ALIGN(getRamBlock(), PAGE_SIZE);

    mapPage(addr, addr, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE | PAGE_FLAG_WT);

    return addr;
}

void freePage(void * va){
    // TODO: controlar el addr que te pasan

    uint32 pageDirPos = ((uint32)va & VA_TO_PD_MASK) >> 22;
    uint32 pageTablePos = ((uint32)va & VA_TO_PT_MASK) >> 12;

    pageTableEntry *pageTable = (pageTableEntry *)(pageDirectory[pageDirPos] & PDE_TO_PT_MASK);

    uint32 pa =  pageTable[pageTablePos];
    uint32 addr = (pa & PA_TO_ADDR_MASK) |  (uint32)va & VA_TO_OFFSET;

    pageTable[pageTablePos] = PAGE_FLAG_NO_PRESENT;

    freeBlock(addr);

}