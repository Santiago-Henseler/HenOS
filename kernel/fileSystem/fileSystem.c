#include "fileSystem.h"

typedef struct superBlock{
    uint32 code; 
    uint32 inodes; // Canitdad de inodes
    uint32 root;
    uint8 bitMap[FS_BITMAP_SIZE]; // 0 indica un bloque libre
} superBlock;

typedef struct dentry{
    char name[FILE_NAME_SIZE];
    uint16 inode;
} dentry;

typedef struct inode{
    fileType type;
    uint16 size; // Total en bytes
    uint16 block[FILE_MAX_BLOCKS];
} inode;

static superBlock sb;
superBlock *sBlock = &sb;

inode *root;

int getFreeBlock(){
    int freeBlock = -1;
    
    int i = 0;
    bool found = false;
    while( i < FS_BITMAP_SIZE && !found){
        if(sBlock->bitMap[i] != 0xFF){ // Si hay un bloque libre en esta parte del bitMap
            int j = 0;
            while(j < 8 && !found){ // Busco cual esta libre de los 8 de esta parte del bitMap
                if((sBlock->bitMap[i] & (1 << (7 - j))) == false){ 
                    freeBlock = i*8 + j;
                    sBlock->bitMap[i] = sBlock->bitMap[i] | (1 << 7-j); // Lo marco como en uso
                    found = true;
                }
                j++;
            }
        }
        i++;
    }

    if(freeBlock == -1)
        return freeBlock;

    freeBlock += SUPER_BLOCK_POS + 1;
    
    // Guardo la nueva info en el superBloque 
    writeFloppyDisk(SUPER_BLOCK_POS, sBlock); 

    return freeBlock;
}

// Devuelve devuelve el numero de bloque del inodo con nombre filename dentro del Current Work Directory
// Si no lo encuentra retorna -1
int findBlockInodeInCWD(char * fileName){
    //TODO: Esto hay que hacerlo con el current directory
    int inode = -1;

    int i = 0;
    bool found = false;    
    while(!found && i < MAX_DENTRY_PER_BLOCK && ((int)(root->size / sizeof(dentry)) - (i * MAX_DENTRY_PER_BLOCK)) > 0){
        uint8 buffer[FLOPPY_BLOCK_SIZE];
        int err = readFloppyDisk(root->block[i], buffer);
        if(err == -1) return -1;
        
        // El total de dentrys a que tiene este bloque
        int remainDentrys = (root->size/sizeof(dentry)) - i*MAX_DENTRY_PER_BLOCK;
        int actualDentrys = MAX_DENTRY_PER_BLOCK;
        if(remainDentrys > 0 && remainDentrys < MAX_DENTRY_PER_BLOCK){
            actualDentrys = remainDentrys;
        }

        dentry *entries = (dentry *)buffer;
        for(int j = 0; j < actualDentrys && !found; j++){ 
            dentry entrie = entries[j];
            if(strCompare(entrie.name, fileName)){
                inode = entrie.inode;
                found = true;
            }
        }
        i++;
    }
    return inode;
}

// Devuelve devuelve el puntero del inodo con nombre filename dentro del Current Work Directory
// Si no lo encuentra retorna NULL
inode * findInodeInCWD(char * fileName){
    int inodeBlockNum = findBlockInodeInCWD(fileName);
    if(inodeBlockNum == -1) return NULL;

    uint8 buffer[FLOPPY_BLOCK_SIZE];
    int err = readFloppyDisk(inodeBlockNum, buffer);
    if(err == -1) return NULL;

    inode * file = malloc(sizeof(inode));
    memCopy(buffer, file, sizeof(inode));
    return file;
}

inode * createInode(fileType type){
    inode * newInode = (inode *)malloc(sizeof(inode));
    if(newInode == NULL)
        return NULL;

    newInode->type = type;
    newInode->size = 0;
    for(int i = 0; i < FILE_MAX_BLOCKS; i++)
        newInode->block[i] = 0;

    return newInode;
}

int createFile(fileType type, char * fileName){
    //TODO: Esto hay que hacerlo con el current directory
    int blockPos = getFreeBlock();
    if(blockPos == -1) goto error0;

    inode * newInode = createInode(type);
    if(newInode == NULL) goto error0;
    
    // Creo el dentry
    dentry newDentry;
    strCopy(fileName, newDentry.name);
    newDentry.inode = blockPos;

    // Guardo el dentry en el inodo tipo dir actual
    if(root->size >= FLOPPY_BLOCK_SIZE * FILE_MAX_BLOCKS) goto error1;

    int dentryBlock = root->size / FLOPPY_BLOCK_SIZE;
    int dentryOffset = root->size % FLOPPY_BLOCK_SIZE;
    if(root->block[dentryBlock] == 0){
        int freeBlock = getFreeBlock();
        if(freeBlock == -1) goto error1;
        root->block[dentryBlock] = freeBlock;
    }

    // Guardo el dentry en disco
    uint8 dentryBuff[FLOPPY_BLOCK_SIZE];
    int floppyErr = readFloppyDisk(root->block[dentryBlock], dentryBuff);
    if(floppyErr == -1) goto error1;
    
    memCopy(&newDentry, dentryBuff + dentryOffset, sizeof(dentry));
    floppyErr = writeFloppyDisk(root->block[dentryBlock], dentryBuff);
    if(floppyErr == -1) goto error1;

    // Guardo el nuevo inodo en disco
    floppyErr = writeFloppyDisk(blockPos, newInode);
    if(floppyErr == -1) goto error1;

    // Actualizo el inodo tipo dir actual
    root->size += sizeof(dentry);
    floppyErr = writeFloppyDisk(SUPER_BLOCK_POS+1, root);
    if(floppyErr == -1) goto error1;

    free(newInode);
    return 0;

    error1: free(newInode);
    error0: return -1;
}

int writeFile(char * fileName, uint8 * data, uint32 size){
    if(fileName == NULL || strLen(fileName) >= FILE_NAME_SIZE)
        return -1;

    // Obtengo el inodo
    int blockNum = findBlockInodeInCWD(fileName);
    if(blockNum == -1) return -1;

    uint8 buffer[FLOPPY_BLOCK_SIZE];
    int err = readFloppyDisk(blockNum, buffer);
    if(err == -1) return -1;

    inode * file = malloc(sizeof(inode));
    memCopy(buffer, file, sizeof(inode));
    if(file == NULL) goto error;
    if(file->type != DATA) goto error;

    // Escribo toda la data en los bloques
    int dataWrited = 0;
    while(dataWrited < size){
        if(file->size + size > FILE_MAX_SIZE) goto error;

        int block = file->size / FLOPPY_BLOCK_SIZE;
        int offset = file->size % FLOPPY_BLOCK_SIZE;
        if(file->block[block] == 0){
            int freeBlock = getFreeBlock();
            if(freeBlock == -1) goto error;
            file->block[block] = freeBlock;
        }
            
        uint8 buff[FLOPPY_BLOCK_SIZE];
        int floppyErr = readFloppyDisk(file->block[block], buff);
        if(floppyErr == -1) goto error;

        int dataLen = size - dataWrited > FLOPPY_BLOCK_SIZE - offset ? FLOPPY_BLOCK_SIZE - offset : size - dataWrited;
        memCopy(data+dataWrited, buff+offset, dataLen);

        floppyErr = writeFloppyDisk(file->block[block], buff);
        if(floppyErr == -1) goto error;

        dataWrited += dataLen;
        file->size += dataLen;
    }

    // Guardo el inodo actualizado
    err = writeFloppyDisk(blockNum, file);
    if(err == -1) goto error;

    return 0;
    
    error: free(file);
    return -1;
}

void * readFile(char * fileName){
    if(fileName == NULL || strLen(fileName) >= FILE_NAME_SIZE)
        return NULL;

    inode * file = findInodeInCWD(fileName);
    if(file == NULL) return NULL;
    if(file->type != DATA) return NULL;

    int read = 0;
    int lastDataBlock  = file->size / FLOPPY_BLOCK_SIZE;
    int blockNum = 0;
    while (read <= file->size && blockNum <= lastDataBlock){
        uint8 buff[FLOPPY_BLOCK_SIZE];
        readFloppyDisk(file->block[blockNum], buff);
        // TODO: guardarlo y devolverlo en un puntero
        printf("%s", buff);

        blockNum++;
        read += FLOPPY_BLOCK_SIZE;
    }

    return NULL;
}

void initFileSystem(){
    uint8 buffer[FLOPPY_BLOCK_SIZE];
    readFloppyDisk(SUPER_BLOCK_POS, buffer);

    memCopy(buffer, sBlock, sizeof(superBlock));

    if(sBlock->code != FS_CODE){ // No hay filesystem creado
        sBlock->code = FS_CODE; 
        sBlock->inodes = 1;
        for(int i = 0; i < FS_BITMAP_SIZE; i++){
            sBlock->bitMap[i] = 0;
        }
        sBlock->root = SUPER_BLOCK_POS+1;
        writeFloppyDisk(SUPER_BLOCK_POS, sBlock); 

        inode * newInode = createInode(DIR);
        getFreeBlock();
        writeFloppyDisk(SUPER_BLOCK_POS+1, newInode);
        root = newInode;
    }else{
        readFloppyDisk(sBlock->root, buffer);
        memCopy(buffer, root, sizeof(inode));
    }
}
