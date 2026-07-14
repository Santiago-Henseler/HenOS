global runProcess
runProcess:
    mov eax, [esp+4] ;; Puntero a codeSegment
    call eax
    ret