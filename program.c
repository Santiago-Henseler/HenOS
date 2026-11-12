void putChar(char c);

void _start(){

    //putChar('c');

    register int ebx asm("ebx") = 'c';

    asm volatile (
        "mov eax, 0x2\n\t"
        "int 0x30\n\t"
        :
        : "b"(ebx)
        : "eax", "memory"
    );

    while (1)
    {
       continue;
    }
    


    return;
}