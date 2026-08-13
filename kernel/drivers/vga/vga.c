#include "vga.h"

int cursorX = 0;
int cursorY = 0;

void moverCursor(int pos){

    if (pos < 0)
        pos = 0;
    else
        pos = pos % (VGA_HEIGHT*VGA_WIDTH);

    // Seteo para enviar parte alta de pos y lo envio
    outB(COMMAND_PORT_VGA, 0xF); 
    outB(DATA_PORT_VGA, ((pos >> 8) & 0xFF));
    // Seteo para enviar parte baja de pos y lo envio
    outB(COMMAND_PORT_VGA, 0xE);
    outB(DATA_PORT_VGA, pos & 0xFF);

    // Dibujo un '_' para indicar donde esta el cursor
    volatile char * videoMem = MEM_VGA + 2*(cursorX + VGA_WIDTH*cursorY);
    videoMem[0] = '_';
    videoMem[1] =  WHITE + BLUE_BG;
}

void caracterVacio(int color){
    volatile char * videoMem = MEM_VGA + 2*(cursorX + VGA_WIDTH*cursorY);
    videoMem[0] = ' ';
    videoMem[1] = BLUE_BG;
    cursorX++;
}

void scrollUpVga(){
   
    for(int y = 0; y < VGA_HEIGHT; y++){
        for(int x = 0; x < VGA_WIDTH; x++){
            volatile char * videoMem = MEM_VGA + 2*(x + VGA_WIDTH*y);
            volatile char * videoMemOld = MEM_VGA +2*(x + VGA_WIDTH*(y+1));
            videoMem[0] = videoMemOld[0];
            videoMem[1] = videoMemOld[1]; 
        }
    }
}

void newLineVga(){

    if(cursorY < VGA_HEIGHT-1){
        cursorY++;
        cursorX = 0;
    }else{
        scrollUpVga();
        cursorX = 0;
    }

    moverCursor(cursorX + VGA_WIDTH*cursorY);
}

void clearVga(){
    for(int y = 0; y < VGA_HEIGHT; y++){
        for(int x = 0; x < VGA_WIDTH; x++){
            volatile char * videoMem = MEM_VGA + 2*x + 160*y;
            videoMem[0] = ' ';
            videoMem[1] = BLUE_BG;
        }
    }
    
    cursorX = 0; cursorY = 0;
    moverCursor(0);
}

void printVga(const char* str, int color){
    while (*str)
    {
        if(*str == KEY_ENTER){
            newLineVga();
            str++;
            continue;
        }else if(*str == KEY_TAB){
            for(int i = 0; i < 4; i++)
                caracterVacio(color);
            str++;
            continue;
        }else if(*str == KEY_ESPACIO){
            caracterVacio(color);
            str++;
            continue;
        }

        volatile char * videoMem = MEM_VGA + 2*(cursorX + VGA_WIDTH*cursorY);

        videoMem[0] = *str++;
        videoMem[1] = color + BLUE_BG; 

        cursorX++;
        if(cursorX == VGA_WIDTH){
            cursorX = 0;
            cursorY++;
        }
        if(cursorY == VGA_HEIGHT)
            scrollUpVga();
    }

    moverCursor(cursorX + VGA_WIDTH*cursorY);
}
