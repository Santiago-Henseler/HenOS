#include "interrupts.h"

char * interruptName[SOFTWARE_INT] = {
    "Se intento dividir por 0", "Debug int", "NMI int",
    "Breackpoint int", "Ocurrio un overflow", "Operacion se fue de rango",
    "Codigo de operación invalido", "PC sin coprocesador", "Doble fallo",
    "", "TSS invalido", "Segmento no presente",
    "Fallo de segmento del stack", "Referencia a memoria protegida", "Fallo de pagina",
    "", "Error en la unidad de punto flotante", "",
    "", "", "", "", "","","","","","","","","",""
};

void interruptSoftware(InterruptRegisters * interruptRegs){

    printf("Interrupt: %i Code: %x \n", interruptRegs->interrupt, interruptRegs->code);
    printf("%s \n", interruptName[interruptRegs->interrupt]);
    printf("eip => %x  esp => %x  flags => %x \n", interruptRegs->eip, interruptRegs->esp, interruptRegs->eflags);

    for(;;); // TODO: Notificar y cambiar de proceso
}

void interruptHardware(InterruptRegisters * interruptRegs){
    
    printf("Interrupt: %i \n", interruptRegs->interrupt);

    // ACK de la interrupcion al PIC
    ackPic(interruptRegs->interrupt-SOFTWARE_INT);
}

void keyboardInterrupt(InterruptRegisters *interruptRegs){
    
    char key = getKeyInput();
    // TODO: en vez de printear enviar al proceso que esta corriendo
    putChar(key);

    // ACK de la interrupcion al PIC
    ackPic(interruptRegs->interrupt-SOFTWARE_INT);
}

void clockInterrupt(InterruptRegisters * interruptRegs){
    // TODO: cambiar de contexto cuando haya multitask

    // ACK de la interrupcion al PIC
    ackPic(interruptRegs->interrupt-SOFTWARE_INT);
}

void floppyDiskInterrupt(InterruptRegisters * interruptRegs){
    setFloppyInt();
    ackPic(interruptRegs->interrupt-SOFTWARE_INT);
}

// Hardware interrupt handlers
void (* hardwareHandlers[HARDWARE_INT])(InterruptRegisters * interruptRegs) = {
    &clockInterrupt, &keyboardInterrupt, &interruptHardware,
    &interruptHardware, &interruptHardware, &interruptHardware,
    &floppyDiskInterrupt, &interruptHardware, &interruptHardware,
    &interruptHardware, &interruptHardware, &interruptHardware,
    &interruptHardware, &interruptHardware, &interruptHardware,
    &interruptHardware
};

void interrupthandler(InterruptRegisters * interruptRegs){

    if(interruptRegs->interrupt > (SOFTWARE_INT + HARDWARE_INT)){
        printf("[Error]: interrupción no soportada\n");
        for(;;);
    }

    if(interruptRegs->interrupt == 31){
        printf("%i \n", interruptRegs->eax);
        printf("%x \n", interruptRegs->eip);

        actualProcess->ip = interruptRegs->eip;

    }else if(interruptRegs->interrupt > SOFTWARE_INT-1){
        hardwareHandlers[interruptRegs->interrupt-SOFTWARE_INT](interruptRegs);
    }else{
        interruptSoftware(interruptRegs);
    }
}
