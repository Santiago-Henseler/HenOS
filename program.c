extern void putChar(char c);
extern void syscall(int interrupt);

void _start(){
    syscall(67);
    putChar('A');
    return;
}