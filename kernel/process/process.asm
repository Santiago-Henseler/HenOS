global runProcess
runProcess:
    mov eax, [esp+4] ;; Puntero al ip
    call eax
    ret