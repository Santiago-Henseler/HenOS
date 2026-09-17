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

typedef struct nodeDirectory{
    dentry * selfDentry;
    struct nodeDirectory * previous;
    struct nodeDirectory ** nodeDirectorys;
    uint16 totalNodeDirectorys;
} nodeDirectory;

static superBlock sb;
superBlock *sBlock = &sb;

// TODO: esto deberia de ser parte de cada proceso
inode *cwd;
int cwdBlock;
nodeDirectory * memFs;

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
    if(writeFloppyDisk(SUPER_BLOCK_POS, sBlock) == -1)
        return -1;

    return freeBlock;
}

// Devuelve devuelve el numero de bloque del inodo con nombre filename dentro del Current Work Directory
// Si no lo encuentra retorna -1
int findBlockInodeInCWD(char * fileName){
    int inodeBlock = -1;

    bool found = false;
    for(int i = 0; i < memFs->totalNodeDirectorys && !found; i++){
        if(strCompare(memFs->nodeDirectorys[i]->selfDentry->name, fileName)){
            inodeBlock = memFs->nodeDirectorys[i]->selfDentry->inodeBlock;
            found = true;
        }
    }

    return inodeBlock;
}

// Devuelve devuelve el puntero del inodo con nombre filename dentro del Current Work Directory
// Si no lo encuentra retorna NULL
inode * findInodeInCWD(char * fileName){
    int inodeBlockNum = findBlockInodeInCWD(fileName);
    if(inodeBlockNum == -1) return NULL;

    uint8 buffer[FLOPPY_BLOCK_SIZE];
    if(readFloppyDisk(inodeBlockNum, buffer) == -1) 
        return NULL;

    inode * file = malloc(sizeof(inode));
    memCopy(buffer, file, sizeof(inode));
    return file;
}

inode * createInode(fileType type){
    inode * newInode = (inode *)calloc(sizeof(inode));
    if(newInode == NULL)
        return NULL;

    newInode->type = type;
    return newInode;
}

int createFile(fileType type, char * fileName){
    if(fileName == NULL) return -1;

    int blockPos = getFreeBlock();
    if(blockPos == -1) goto error0;

    inode * newInode = createInode(type);
    if(newInode == NULL) goto error0;
    
    // Creo el dentry
    dentry newDentry;
    strCopy(fileName, newDentry.name);
    newDentry.inodeBlock = blockPos;

    if(cwd->size == FILE_MAX_SIZE || findBlockInodeInCWD(fileName) != -1) 
        goto error1;

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
    if(readFloppyDisk(cwd->block[dentryBlock], dentryBuff) == -1) 
        goto error1;
    
    memCopy(&newDentry, dentryBuff + dentryOffset, sizeof(dentry));
    if(writeFloppyDisk(cwd->block[dentryBlock], dentryBuff) == -1) 
        goto error1;

    // Guardo el nuevo inodo en disco
    if(writeFloppyDisk(blockPos, newInode) == -1) 
        goto error1;

    // Actualizo el cwd
    cwd->size += sizeof(dentry);
    if(writeFloppyDisk(cwdBlock, cwd) == -1) 
        goto error1;

    if (updateNodeDirectory(memFs, &newDentry) == -1)
        goto error1;

    free(newInode);
    return 0;

    error1: free(newInode);
    error0: return -1;
}

int writeFile(char * fileName, uint8 * bytes, uint32 size){
    if(fileName == NULL || strLen(fileName) >= FILE_NAME_SIZE || bytes == NULL)
        return -1;

    // Obtengo el inodo
    int blockNum = findBlockInodeInCWD(fileName);
    if(blockNum == -1) return -1;

    uint8 buffer[FLOPPY_BLOCK_SIZE];
    int err = readFloppyDisk(blockNum, buffer);
    if(err == -1) return -1;

    inode * file = malloc(sizeof(inode));
    if(file == NULL) goto error;
    if(file->type != DATA) goto error;
    memCopy(buffer, file, sizeof(inode));

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
        if(readFloppyDisk(file->block[block], buff) == -1) goto error;

        int bytesLen = size - bytesWrited > FLOPPY_BLOCK_SIZE - offset ? FLOPPY_BLOCK_SIZE - offset : size - bytesWrited;
        memCopy(bytes+bytesWrited, buff+offset, bytesLen);

        if(writeFloppyDisk(file->block[block], buff) == -1) goto error;

        bytesWrited += bytesLen;
        file->size += bytesLen;
    }

    // Guardo el inodo actualizado
    if(writeFloppyDisk(blockNum, file) == -1) goto error;

    return 0;
    
    error: free(file);
    return -1;
}

void * readFile(char * fileName){
    if(fileName == NULL || strLen(fileName) >= FILE_NAME_SIZE)
        return NULL;

    inode * file = findInodeInCWD(fileName);
    if(file == NULL) return NULL;
    if(file->type == DIR) return NULL;

    int read = 0;
    int lastDataBlock  = file->size / FLOPPY_BLOCK_SIZE;
    int blockNum = 0;
    while (read <= file->size && blockNum <= lastDataBlock){
        uint8 buff[FLOPPY_BLOCK_SIZE];
        if(readFloppyDisk(file->block[blockNum], buff) == -1) return NULL;
        // TODO: guardarlo y devolverlo en un puntero
        printf("%s", buff);

        blockNum++;
        read += FLOPPY_BLOCK_SIZE;
    }

    return NULL;
}

nodeDirectory * newNodeDirectory(){
    nodeDirectory * nodeDir = (nodeDirectory *)malloc(sizeof(nodeDirectory));
    if(nodeDir == NULL)
        return NULL;

    nodeDir->selfDentry = (dentry *)malloc(sizeof(dentry));
    nodeDir->previous = (nodeDirectory *)malloc(sizeof(nodeDirectory * ));
    nodeDir->nodeDirectorys = malloc(sizeof(nodeDirectory *));
    if(nodeDir->selfDentry == NULL || nodeDir->previous == NULL || nodeDir->nodeDirectorys == NULL)
        return NULL;

    nodeDir->nodeDirectorys[0] = (nodeDirectory *)malloc(sizeof(nodeDirectory));
    if(nodeDir->nodeDirectorys[0] == NULL)
        return NULL;

    nodeDir->totalNodeDirectorys = 0;
    
    return nodeDir;
}

// Levanta una estructura similar a un arbol para operar de manera mas rapida con el filesystem
// selfDentry == NULL && previous == NULL para indicar que se empieza desde root
nodeDirectory * initNodeDirectory(uint16 inodeBlock, dentry * selfDentry, nodeDirectory * previous){
    nodeDirectory * nodeDir = newNodeDirectory();
    
    if(nodeDir == NULL)
        return NULL;

    if(selfDentry != NULL){
        memCopy(selfDentry, nodeDir->selfDentry, sizeof(dentry)); 
    }else if(selfDentry == NULL && previous == NULL){
        nodeDir->previous = nodeDir;
        nodeDir->selfDentry = NULL; //TODO: crearle uno
    }

    inode actualInode; 
    uint8 buffer[FLOPPY_BLOCK_SIZE];
    if(readFloppyDisk(inodeBlock, buffer) == -1)
        return NULL;
    memCopy(buffer, &actualInode, sizeof(inode));

    if(actualInode.type != DIR)
        return nodeDir;
   
    int i = 0;
    while(((int)(actualInode.size / sizeof(dentry)) - (i * MAX_DENTRY_PER_BLOCK)) > 0){
        uint8 buffer[FLOPPY_BLOCK_SIZE];
        if(readFloppyDisk(actualInode.block[i], buffer) == -1) return NULL;
        
        int remainDentrys = (int)(actualInode.size/sizeof(dentry)) - i * MAX_DENTRY_PER_BLOCK;
        int actualDentrys = MAX_DENTRY_PER_BLOCK;
        if(remainDentrys > 0 && remainDentrys < MAX_DENTRY_PER_BLOCK){
            actualDentrys = remainDentrys;
        }
        
        dentry *entries = (dentry *)buffer;
        for(int j = 0; j < actualDentrys; j++){ 
            dentry entrie = entries[j];
            nodeDir->totalNodeDirectorys++;
            if(j != 0){
                nodeDir->nodeDirectorys = realloc(nodeDir->nodeDirectorys, nodeDir->totalNodeDirectorys * sizeof(nodeDirectory *));
                if(nodeDir->nodeDirectorys == NULL)
                    return NULL;
            }
            nodeDir->nodeDirectorys[nodeDir->totalNodeDirectorys-1] = initNodeDirectory(entrie.inodeBlock, &entrie, nodeDir);
        }
        i++;
    }
    return nodeDir;
}

int updateNodeDirectory(nodeDirectory * actual, dentry * newDentry){
    actual->totalNodeDirectorys++;

    nodeDirectory * newNode = newNodeDirectory();
    if(newNode == NULL)
        return -1;

    newNode->previous = actual;
    memCopy(newDentry, newNode->selfDentry, sizeof(dentry));
    
    actual->nodeDirectorys = realloc(actual->nodeDirectorys, actual->totalNodeDirectorys * sizeof(nodeDirectory *));
    if(actual->nodeDirectorys == NULL)
        return -1;

    actual->nodeDirectorys[actual->totalNodeDirectorys-1] = newNode;
    return 0;
} 

void initFileSystem(){
    uint8 buffer[FLOPPY_BLOCK_SIZE];
    if(readFloppyDisk(SUPER_BLOCK_POS, buffer)) 
        kernelPanic();

    memCopy(buffer, sBlock, sizeof(superBlock));

    if(sBlock->code != FS_CODE){ // No hay filesystem creado
        sBlock->code = FS_CODE; 
        sBlock->inodes = 1;
        for(int i = 0; i < FS_BITMAP_SIZE; i++){
            sBlock->bitMap[i] = 0;
        }
        sBlock->root = SUPER_BLOCK_POS+1;
        if(writeFloppyDisk(SUPER_BLOCK_POS, sBlock) == -1) 
            kernelPanic(); 

        inode * newInode = createInode(DIR);
        if(newInode == NULL || getFreeBlock() == -1) 
            kernelPanic();
        if(writeFloppyDisk(SUPER_BLOCK_POS+1, newInode) == -1) 
            kernelPanic();

        cwd = newInode;
        cwdBlock = SUPER_BLOCK_POS+1;
    }else{
        readFloppyDisk(sBlock->root, buffer);
        memCopy(buffer, cwd, sizeof(inode));
        cwdBlock = SUPER_BLOCK_POS+1;
    }

    nodeDirectory * nodeDir = initNodeDirectory(SUPER_BLOCK_POS+1, NULL, NULL);
    if(nodeDir == NULL)
        kernelPanic();

    memCopy(nodeDir, memFs, sizeof(nodeDirectory));
}