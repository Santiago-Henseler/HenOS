#include "floppy.h"

static uint8 floppyDmaBuffer[FLOPPY_BLOCK_SIZE] __attribute__((aligned(0x1000)));

typedef struct floppyConfig{
    uint8 sectors;
    uint8 cylinders;
} floppyConfig;

floppyConfig * floppyInfo1 = NULL;
floppyConfig * floppyInfo2 = NULL;

bool floppyInterrupt = false;
void setFloppyInt(){
    floppyInterrupt = true;
}

void waitFloppyInt(){
    while(!floppyInterrupt)
        continue;
    floppyInterrupt = false;
}

uint8 reciveFloppyCommand(){

    bool available = false;

    while (!available) {
        uint8 msr = inB(FLOPPY_STATUS_PORT);
        if ((msr & 0x80) && (msr & 0x40))
            available = true;
    }

    return inB(FLOPPY_DATA_PORT);
}

void sendFloppyCommand(uint8 command){

    bool available = false;

    while (!available) {
        uint8 msr = inB(FLOPPY_STATUS_PORT);
        if ((msr & 0x80) && !(msr & 0x40))
            available = true;
    }

    outB(FLOPPY_DATA_PORT, command);
}

void floppySeek(uint8 cylinder, uint8 head, uint8 driver) {
    sendFloppyCommand(FLOPPY_SEEK_COMMAND);
    sendFloppyCommand((head << 2) | driver);
    sendFloppyCommand(cylinder);

    waitFloppyInt();

    sendFloppyCommand(FLOPPY_SENSE_COMMAND);
    reciveFloppyCommand();  
    reciveFloppyCommand(); 
}

void floppyReset(){

    uint8 dor = inB(FLOPPY_DIGITAL_OUTPUT_PORT);

    outB(FLOPPY_DIGITAL_OUTPUT_PORT, dor & ~0x04);

    ioWait(4);

    outB(FLOPPY_DIGITAL_OUTPUT_PORT, dor | 0x04);

    waitFloppyInt();

    for (int i = 0; i < 4; i++) {
        sendFloppyCommand(FLOPPY_SENSE_COMMAND);
        reciveFloppyCommand();
        reciveFloppyCommand();
    }

    outB(FLOPPY_DIGITAL_OUTPUT_PORT, 0x00);
    outB(FLOPPY_DIGITAL_OUTPUT_PORT, 0x0c);
    outB(FLOPPY_DIGITAL_OUTPUT_PORT, 0x1C);

    waitFloppyInt();
}

int floppyDiskAction(int blockNum, uint8 buffer[FLOPPY_BLOCK_SIZE], bool write){

    uint8 driver = 0;
    uint8 head = (blockNum / floppyInfo1->sectors) % FLOPPY_HEADS;
    uint8 cylinder = blockNum / (floppyInfo1->sectors * FLOPPY_HEADS);
    uint8 sector = (blockNum % floppyInfo1->sectors) + 1;

    if(blockNum >= floppyInfo1->cylinders * floppyInfo1->sectors * FLOPPY_HEADS)
        if(floppyInfo2 != NULL)
            if(blockNum >= floppyInfo1->cylinders * floppyInfo1->sectors * FLOPPY_HEADS + floppyInfo2->cylinders * floppyInfo2->sectors * FLOPPY_HEADS)
                return -1;
            else{
                return -1;
                // TODO
                driver = 1;
                head = (blockNum - (floppyInfo1->cylinders * floppyInfo1->sectors * FLOPPY_HEADS) / floppyInfo2->sectors) % FLOPPY_HEADS;
                cylinder = (blockNum - (floppyInfo1->cylinders * floppyInfo1->sectors * FLOPPY_HEADS)) / (floppyInfo2->sectors * FLOPPY_HEADS);
                sector = (blockNum- (floppyInfo1->cylinders * floppyInfo1->sectors * FLOPPY_HEADS) % floppyInfo2->sectors) + 1;
            }
        else
            return -1;

    if(write)
        memCopy(buffer, floppyDmaBuffer, FLOPPY_BLOCK_SIZE);

    setupDMA(floppyDmaBuffer, FLOPPY_BLOCK_SIZE, write ? DMA_WRITE : DMA_READ);


    floppySeek(cylinder, head, 0);

    // Indico la accion a ejecutar
    sendFloppyCommand(write ? (FLOPPY_MT | FLOPPY_MFM | 0x5) : (FLOPPY_MT | FLOPPY_MFM | 0x6 ));

    sendFloppyCommand((head << 2) | 0); // ver driver number (el 0)
    sendFloppyCommand(cylinder);
    sendFloppyCommand(head);
    sendFloppyCommand(sector);
    sendFloppyCommand(2);
    sendFloppyCommand(18);
    sendFloppyCommand(0x1B);
    sendFloppyCommand(0xFF);

    waitFloppyInt(); 

    uint8 st0 = reciveFloppyCommand();
    uint8 st1 = reciveFloppyCommand();
    uint8 st2 = reciveFloppyCommand();
    reciveFloppyCommand();
    reciveFloppyCommand();
    reciveFloppyCommand();
    reciveFloppyCommand();

    if ((st0 & 0xC0) != 0) return -1;
    if (st1 | st2) return -1;

    if(!write)
        memCopy(floppyDmaBuffer, buffer, FLOPPY_BLOCK_SIZE);

    return 0;
}

int writeFloppyDisk(int blockNum, uint8 buffer[FLOPPY_BLOCK_SIZE]){
    return floppyDiskAction(blockNum, buffer, true); 
}

int readFloppyDisk(int blockNum, uint8 buffer[FLOPPY_BLOCK_SIZE]){
    return floppyDiskAction(blockNum, buffer, false);
}

int setupInfoFloppyDisk(){
    outB(CMOS_FLOPPY_INFO_OUT, CMOS_FLOPPY_INFO);
    uint8 size = inB(CMOS_FLOPPY_INFO_IN);

    floppyConfig * infoFloppy1 = (floppyConfig *)malloc(sizeof(floppyConfig));
    if(infoFloppy1 == NULL)
        return -1;

    // Chequero por el primer floppy
    if(size >> 4 == FLOPPY_360KB){
        infoFloppy1->cylinders = FLOPPY_CYLINDERS_360KB;
        infoFloppy1->sectors = FLOPPY_SECTORS_360KB;
    }else if(size >> 4 == FLOPPY_720KB){
        infoFloppy1->cylinders = FLOPPY_CYLINDERS_720KB;
        infoFloppy1->sectors = FLOPPY_SECTORS_720KB;
    }else if(size >> 4 == FLOPPY_144MB){
        infoFloppy1->cylinders = FLOPPY_CYLINDERS_144MB;
        infoFloppy1->sectors = FLOPPY_SECTORS_144MB;
    }else if(size >> 4 == FLOPPY_288MB){
        infoFloppy1->cylinders = FLOPPY_CYLINDERS_288MB;
        infoFloppy1->sectors = FLOPPY_SECTORS_288MB;
    }else{
        return -1;
    }
    floppyInfo1 = infoFloppy1;

    // Chequeo si hay un segundo floppy
    uint8 secondFloppySize = size & 0x0F;
    if(secondFloppySize != 0){
        floppyConfig * infoFloppy2 = (floppyConfig *)malloc(sizeof(floppyConfig));
        if(infoFloppy2 == NULL)
            return -1;

        if(secondFloppySize == FLOPPY_360KB){
            infoFloppy2->cylinders = FLOPPY_CYLINDERS_360KB;
            infoFloppy2->sectors = FLOPPY_SECTORS_360KB;
        }else if(secondFloppySize == FLOPPY_720KB){
            infoFloppy2->cylinders = FLOPPY_CYLINDERS_720KB;
            infoFloppy2->sectors = FLOPPY_SECTORS_720KB;
        }else if(secondFloppySize == FLOPPY_144MB){
            infoFloppy2->cylinders = FLOPPY_CYLINDERS_144MB;
            infoFloppy2->sectors = FLOPPY_SECTORS_144MB;
        }else if(secondFloppySize == FLOPPY_288MB){
            infoFloppy2->cylinders = FLOPPY_CYLINDERS_288MB;
            infoFloppy2->sectors = FLOPPY_SECTORS_288MB;
        }else{
            return -1;
        }

        floppyInfo2 = infoFloppy2;
    }

    return 0;    
}

void initFloppyDisk(){

    if(setupInfoFloppyDisk() == -1)
        kernelPanic();

    sendFloppyCommand(FLOPPY_VERSION_COMMAND);
    uint8 version = reciveFloppyCommand();

    if(version != 0x90){
        printVga("No hay soporte para esa version de floppy disk", RED);
        kernelPanic();
    }
    // Configuro el floppy disk
    sendFloppyCommand(FLOPPY_CONFIGURE_COMMAND);
    sendFloppyCommand(0x68);   // Busqueda implicita habilitada              
    sendFloppyCommand(0x0F);   // FIFO habilitado
    sendFloppyCommand(0x00);   // Uso de valor predeterminado de compresion
    
    // Mantener la configuracion en el floppy
    sendFloppyCommand(FLOPPY_LOCK_COMMAND);
    floppyReset();

    // Recalibro el Floppy
    floppyInterrupt = false;
    sendFloppyCommand(FLOPPY_RECALIBRATE_COMMAND);
    sendFloppyCommand(0x00);

    sendFloppyCommand(FLOPPY_SENSE_COMMAND); 
    reciveFloppyCommand();
    reciveFloppyCommand(); 
}