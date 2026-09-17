#ifndef SYSCALLHANDLER

// Headers extra
#include <stdint.h>

#define SYS_WRITE 0x0
#define SYS_READ  0x1

void syscallHandler(uint8 sysCallNum, uint32 p1, uint32 p2, uint32 p3);


#endif