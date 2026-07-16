global syscall
syscall:
    mov eax, [esp+4]        ; Cargo el numero de syscall
    int 0x1f                ; Defini la int 0x1f como la syscall
    ret