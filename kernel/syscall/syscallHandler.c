#include "syscallHandler.h"

int syscallHandler(uint8 sysCallNum, uint32 p1, uint32 p2, uint32 p3){
    switch (sysCallNum){
        case SYS_WRITE:
            return 0;
            break;
        case SYS_READ:
            return 0;
            break;
        case SYS_WRITE_VGA:
            printVga(p1, WHITE);
            return 0;
            break;
        case SYS_MAP_PAGE:  
            return getPage();
            break;
        default:
            return -1;
            break;
    }
}