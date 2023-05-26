//
// Created by AlexHoring on 2023/5/25.
//

#include "FileSystem.h"

FileSystem *FileSystem::instance = nullptr;

FileSystem::FileSystem() {
    disk = DiskDriver::getInstance();
    isOpen = false;
}

FileSystem::~FileSystem() {
    DiskDriver::revokeInstance();
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
//    disk->seekStart(sizeof(capasity));
//    disk->read(reinterpret_cast<char *>(&isUnformatted),sizeof isUnformatted);
//    std::cout<<(int)isUnformatted<<std::endl;
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
    systemInfo.rootLocation = blockStackSize + 1;       //根目录位于空闲块栈的下一个块
    systemInfo.avaliableCapasity = blockSize * (totalBlock - blockStackSize - 2);       //初始可用块个数=总容量-栈大小-引导超级块-根目录
    systemInfo.freeBlockNumber = totalBlock - blockStackSize - 2;
    disk->write(reinterpret_cast<char *>(&systemInfo),sizeof(systemInfo));
    //初始化空闲块栈
    uint32_t ptr=(blockStackSize+1)*blockSize;
    for(uint32_t b = totalBlock - 1; b >= blockStackSize + 2; --b){
        ptr-=sizeof (uint32_t);
        disk->seekStart(ptr);
        disk->write(reinterpret_cast<char *>(&b),sizeof b);
    }
    Directory dir;
    dir.item[0].inodeIndex=0;
    disk->seekStart(systemInfo.rootLocation*blockSize);
    disk->write(reinterpret_cast<char *>(&dir),sizeof dir);
    disk->seekStart(0);
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
        disk->read(reinterpret_cast<char *>(&systemInfo),sizeof systemInfo);
    }
    disk->seekStart(0);
    return true;
}

uint32_t FileSystem::blockAllocate() {
    return 0;
}

void FileSystem::blockFree(uint16_t bno) {

}

void FileSystem::read(uint16_t bno, uint8_t offset, char *buf, uint8_t sz) {

}

void FileSystem::write(uint16_t bno, uint8_t offset, char *buf, uint8_t sz) {

}

bool FileSystem::createDisk(uint32_t sz) {
    return disk->init(sz);
}
