//
// Created by AlexHoring on 2023/5/25.
//

#include "FileSystem.h"

FileSystem *FileSystem::instance = nullptr;

FileSystem::FileSystem() {
    disk = DiskDriver::getInstance();
    stack = new FreeBlockStack();
    isOpen = false;
}

FileSystem::~FileSystem() {
    if (systemInfo.flag) {
        disk->seekStart(sizeof capasity + sizeof isUnformatted + sizeof blockSize);
        systemInfo.flag = 0;
        disk->write(reinterpret_cast<char *>(&systemInfo), sizeof systemInfo);
    }
    DiskDriver::revokeInstance();
    delete stack;
}

FileSystem *FileSystem::getInstance() {
    if (instance == nullptr) {
        instance = new FileSystem;
    }
    return instance;
}

bool FileSystem::format(uint16_t bsize) {
    if (!isOpen) {
        return false;
    }

    //写入格式化标记、块大小
    disk->seekStart(sizeof(capasity));
    isUnformatted = 0;
    disk->write(reinterpret_cast<char *>(&isUnformatted), sizeof isUnformatted);
    blockSize = bsize;
    disk->write(reinterpret_cast<char *>(&blockSize), sizeof blockSize);
    systemInfo.flag = 0;

    systemInfo.freeBlockStackTop = 1;                 //空闲块栈顶初始位于磁盘块1
    uint32_t totalBlock = capasity / blockSize;         //磁盘被划分的块数
    uint32_t blockStackSize = totalBlock * sizeof(uint32_t) / blockSize;  //空闲块栈所占用的磁盘块个数
    if (totalBlock * sizeof(uint32_t) % blockSize != 0) {
        blockStackSize += 1;
    }

    systemInfo.rootLocation = blockStackSize + 1;       //根目录位于空闲块栈底的下一个块
    systemInfo.avaliableCapasity = blockSize * (totalBlock - blockStackSize - 3);       //初始可用块个数=总容量-栈大小-引导超级块-根目录i节点-根目录项
    systemInfo.freeBlockNumber = totalBlock - blockStackSize - 3;       //空闲块个数=总块数-空闲块栈大小-引导块-根目录项

    //初始化磁盘中的空闲块栈，并登记栈顶所在块与偏移
    uint32_t ptr = (blockStackSize + 1) * blockSize;
    std::cout<<ptr<<std::endl;
    for (uint32_t b = totalBlock - 1; b >= blockStackSize + 3; --b) {
        ptr -= sizeof(uint32_t);
        disk->seekStart(ptr);
        disk->write(reinterpret_cast<char *>(&b), sizeof b);
    }

    disk->seekStart(blockSize * 1);
    bool findStackTop = false;
    auto &i = systemInfo.freeBlockStackTop;
    auto &j = systemInfo.freeBlockStackOffset;
    for (i = 1; i <= blockStackSize; ++i) {
        for (j = 0; j < stack->getMaxSize(); ++j) {
            uint32_t t;
            disk->read(reinterpret_cast<char *>(&t), sizeof t);
            if (t != 0) {
                findStackTop = true;
                break;
            }
        }
        if (findStackTop) {
            break;
        }
    }

    disk->write(reinterpret_cast<char *>(&systemInfo), sizeof(systemInfo));
    //写入空闲块栈
    auto blocks = stack->getBlocks();
    disk->seekStart(systemInfo.freeBlockStackTop * blockSize);
    disk->read(reinterpret_cast<char *>(blocks), sizeof(blocks[0]) * stack->getMaxSize());
    stack->setStackTop(systemInfo.freeBlockStackOffset);

    INode rootINode;
    Directory dir;
    rootINode.uid=0;    //0表示系统
    rootINode.bno=systemInfo.rootLocation+1;
    rootINode.flag=0x7f;         //01111111，目录，所有用户都有rwx权限
    dir.item[0].inodeIndex = systemInfo.rootLocation;
    strcpy(dir.item[0].name,".");       //当前目录指向自己
    disk->seekStart(systemInfo.rootLocation * blockSize);
    disk->write(reinterpret_cast<char *>(&rootINode), sizeof rootINode);
    disk->seekStart(rootINode.bno*blockSize);
    disk->write(reinterpret_cast<char *>(&dir), sizeof dir);
    return true;
}

bool FileSystem::mount() {
    if (!disk->open()) {
        return false;
    }
    //读取磁盘容量与是否格式化的信息
    disk->seekStart(0);
    disk->read(reinterpret_cast<char *>(&capasity), sizeof(capasity));
    disk->read(reinterpret_cast<char *>(&isUnformatted), sizeof(isUnformatted));
    isOpen = true;
    if (!isUnformatted) {
        disk->read(reinterpret_cast<char *>(&blockSize), sizeof blockSize);
        disk->read(reinterpret_cast<char *>(&systemInfo), sizeof systemInfo);
        auto blocks = stack->getBlocks();
        disk->seekStart(systemInfo.freeBlockStackTop * blockSize);
        disk->read(reinterpret_cast<char *>(blocks), sizeof(blocks[0]) * stack->getMaxSize());
        stack->setStackTop(systemInfo.freeBlockStackOffset);
    }

    disk->seekStart(0);
    return true;
}

uint32_t FileSystem::blockAllocate() {
    bool isStackEmpty=stack->empty();
    if(isStackEmpty){
        auto blocks=stack->getBlocks();
        systemInfo.freeBlockStackTop++;
        systemInfo.freeBlockStackOffset=0;
        disk->seekStart(systemInfo.freeBlockStackTop*blockSize);
        disk->read(reinterpret_cast<char *>(blocks), sizeof(blocks[0]) * stack->getMaxSize());
        stack->setStackTop(systemInfo.freeBlockStackOffset);
    }
    uint32_t ret = stack->getBlock();
    systemInfo.freeBlockStackOffset++;
    systemInfo.flag=1;
    return ret;
}

void FileSystem::blockFree(uint32_t bno) {
    bool isStackFull=stack->full();
    if(isStackFull){
        auto blocks=stack->getBlocks();
        disk->seekStart(systemInfo.freeBlockStackTop*blockSize);
        disk->write(reinterpret_cast<char *>(blocks), sizeof(blocks[0]) * stack->getMaxSize());
        systemInfo.freeBlockStackTop--;
        systemInfo.freeBlockStackOffset=stack->getMaxSize();
        stack->setStackTop(systemInfo.freeBlockStackOffset);
    }
    stack->revokeBlock(bno);
    systemInfo.flag=1;
}

void FileSystem::read(uint32_t bno, uint16_t offset, char *buf, uint16_t sz) {
    uint32_t base = bno * blockSize;
    disk->seekStart(base + offset);
    disk->read(buf, sz);
}

void FileSystem::write(uint32_t bno, uint16_t offset, char *buf, uint16_t sz) {
    uint32_t base = bno * blockSize;
    disk->seekStart(base + offset);
    disk->write(buf, sz);
}

bool FileSystem::createDisk(uint32_t sz) {
    return disk->init(sz);
}

void FileSystem::readNext(char *buf, uint16_t sz) {
    disk->read(buf, sz);
}

void FileSystem::writeNext(char *buf, uint16_t sz) {
    disk->write(buf, sz);
}

void FileSystem::locale(uint32_t bno, uint16_t offset) {
    uint32_t base = bno * blockSize;
    disk->seekStart(base + offset);
}

void FileSystem::revokeInstance() {
    delete instance;
    instance= nullptr;
}

void FileSystem::update() {
    if(systemInfo.flag==1){
        systemInfo.flag=0;
        //写入基础信息
        disk->seekStart(sizeof(capasity));
        disk->write(reinterpret_cast<char *>(&isUnformatted), sizeof isUnformatted);
        disk->write(reinterpret_cast<char *>(&blockSize), sizeof blockSize);
        disk->write(reinterpret_cast<char *>(&systemInfo), sizeof(systemInfo));
        //写入空闲块栈信息
        auto blocks=stack->getBlocks();
        disk->seekStart(systemInfo.freeBlockStackTop*blockSize);
        disk->write(reinterpret_cast<char *>(blocks), sizeof(blocks[0]) * stack->getMaxSize());
    }
}
