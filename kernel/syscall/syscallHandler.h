#ifndef SYSCALLHANDLER

// Headers extra
#include <stdint.h>
#include <vga/vga.h>
#include <page.h>

#define SYS_WRITE 0x0
#define SYS_READ  0x1
#define SYS_WRITE_VGA 0x2
#define SYS_MAP_PAGE 0x3

int syscallHandler(uint8 sysCallNum, uint32 p1, uint32 p2, uint32 p3);


#endif