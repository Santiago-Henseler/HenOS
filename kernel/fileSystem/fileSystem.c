#include "fileSystem.h"

typedef struct superBlock{
    uint32 code; 
    uint32 inodes; // Canitdad de inodes
    uint32 root;
    uint8 bitMap[FS_BITMAP_SIZE]; // 0 indica un bloque libre
} superBlock;

typedef struct dentry{
    char name[FILE_NAME_SIZE];
    uint16 inodeBlock;
} dentry;

typedef struct inode{
    fileType type;
    uint16 size; // Total en bytes
    uint16 block[FILE_MAX_BLOCKS];
} inode;

static superBlock sb;
superBlock *sBlock = &sb;

// TODO: esto deberia de ser parte de cada proceso
inode *cwd;
int cwdBlock;

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
    int inodeBlock = -1;

    int i = 0;
    bool found = false;    
    while(!found && i < MAX_DENTRY_PER_BLOCK && ((int)(cwd->size / sizeof(dentry)) - (i * MAX_DENTRY_PER_BLOCK)) > 0){
        uint8 buffer[FLOPPY_BLOCK_SIZE];
        int err = readFloppyDisk(cwd->block[i], buffer);
        if(err == -1) return -1;
        
        // El total de dentrys a que tiene este bloque
        int remainDentrys = (cwd->size/sizeof(dentry)) - i*MAX_DENTRY_PER_BLOCK;
        int actualDentrys = MAX_DENTRY_PER_BLOCK;
        if(remainDentrys > 0 && remainDentrys < MAX_DENTRY_PER_BLOCK){
            actualDentrys = remainDentrys;
        }

        dentry *entries = (dentry *)buffer;
        for(int j = 0; j < actualDentrys && !found; j++){ 
            dentry entrie = entries[j];
            if(strCompare(entrie.name, fileName)){
                inodeBlock = entrie.inodeBlock;
                found = true;
            }
        }
        i++;
    }
    return inodeBlock;
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
    int blockPos = getFreeBlock();
    if(blockPos == -1) goto error0;

    inode * newInode = createInode(type);
    if(newInode == NULL) goto error0;
    
    // Creo el dentry
    dentry newDentry;
    strCopy(fileName, newDentry.name);
    newDentry.inodeBlock = blockPos;

    if(cwd->size == FILE_MAX_SIZE || findBlockInodeInCWD(fileName) != -1) goto error1;

    // Guardo el dentry del nuevo file en el cwd
    int dentryBlock = cwd->size / FLOPPY_BLOCK_SIZE;
    int dentryOffset = cwd->size % FLOPPY_BLOCK_SIZE;
    if(cwd->block[dentryBlock] == 0){
        int freeBlock = getFreeBlock();
        if(freeBlock == -1) goto error1;
        cwd->block[dentryBlock] = freeBlock;
    }

    // Guardo el dentry en disco
    uint8 dentryBuff[FLOPPY_BLOCK_SIZE];
    int floppyErr = readFloppyDisk(cwd->block[dentryBlock], dentryBuff);
    if(floppyErr == -1) goto error1;
    
    memCopy(&newDentry, dentryBuff + dentryOffset, sizeof(dentry));
    floppyErr = writeFloppyDisk(cwd->block[dentryBlock], dentryBuff);
    if(floppyErr == -1) goto error1;

    // Guardo el nuevo inodo en disco
    floppyErr = writeFloppyDisk(blockPos, newInode);
    if(floppyErr == -1) goto error1;

    // Actualizo el cwd
    cwd->size += sizeof(dentry);
    floppyErr = writeFloppyDisk(cwdBlock, cwd);
    if(floppyErr == -1) goto error1;

    free(newInode);
    return 0;

    error1: free(newInode);
    error0: return -1;
}

int writeFile(char * fileName, uint8 * bytes, uint32 size){
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
    int bytesWrited = 0;
    while(bytesWrited < size){
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

        int bytesLen = size - bytesWrited > FLOPPY_BLOCK_SIZE - offset ? FLOPPY_BLOCK_SIZE - offset : size - bytesWrited;
        memCopy(bytes+bytesWrited, buff+offset, bytesLen);

        floppyErr = writeFloppyDisk(file->block[block], buff);
        if(floppyErr == -1) goto error;

        bytesWrited += bytesLen;
        file->size += bytesLen;
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
        cwd = newInode;
        cwdBlock = SUPER_BLOCK_POS+1;
    }else{
        readFloppyDisk(sBlock->root, buffer);
        memCopy(buffer, cwd, sizeof(inode));
        cwdBlock = SUPER_BLOCK_POS+1;
    }
}

typedef struct memDirectory{
    dentry ** dentrys;
    uint16 totalDentrys;
} memDirectory;

memDirectory * newMemDirectory(uint16 inodeBlock){
    inode actualInode; 
    uint8 buffer[FLOPPY_BLOCK_SIZE];
    readFloppyDisk(inodeBlock, buffer);
    memCopy(buffer, &actualInode, sizeof(inode));

    memDirectory * memDir = (memDirectory *)malloc(sizeof(memDirectory));
    memDir->totalDentrys = 0;
    memDir->dentrys = malloc(sizeof(dentry *));
    memDir->dentrys[0] = malloc(sizeof(dentry));

    int i = 0;
    while(((int)(actualInode.size / sizeof(dentry)) - (i * MAX_DENTRY_PER_BLOCK)) > 0){
        uint8 buffer[FLOPPY_BLOCK_SIZE];
        int err = readFloppyDisk(actualInode.block[i], buffer);
        if(err == -1) return NULL;
        
        int remainDentrys = (int)(actualInode.size/sizeof(dentry)) - i * MAX_DENTRY_PER_BLOCK;
        int actualDentrys = MAX_DENTRY_PER_BLOCK;
        if(remainDentrys > 0 && remainDentrys < MAX_DENTRY_PER_BLOCK){
            actualDentrys = remainDentrys;
        }
        
        dentry *entries = (dentry *)buffer;
        for(int j = 0; j < actualDentrys; j++){ 
            dentry entrie = entries[j];
            memDir->totalDentrys++;
            if(j != 0)
                memDir->dentrys = realloc(memDir->dentrys, memDir->totalDentrys * sizeof(dentry *));
            memDir->dentrys[memDir->totalDentrys-1] = (dentry *)malloc(sizeof(dentry));
            memCopy(&entrie, memDir->dentrys[memDir->totalDentrys -1], sizeof(dentry)); 
        }
        i++;
    }
    return memDir;
}

void ls(){
    memDirectory * a = newMemDirectory(cwdBlock);

    for(int i = 0; i < a->totalDentrys; i++){
        printf("%s \n", a->dentrys[i]->name); 
    }
}