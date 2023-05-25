//
// Created by AlexHoring on 2023/5/25.
//

#include "FileSystem.h"

FileSystem* FileSystem::instance= nullptr;

FileSystem::FileSystem() {
    disk=DiskDriver::getInstance();
    isOpen= false;
}

FileSystem::~FileSystem() {
    DiskDriver::revokeInstance();
}

FileSystem *FileSystem::getInstance() {
    if(instance== nullptr){
        instance=new FileSystem;
    }
    return instance;
}

bool FileSystem::format(uint16_t bsize) {
    return false;
}

bool FileSystem::mount() {
    return false;
}

uint16_t FileSystem::blockAllocate() {
    return 0;
}

void FileSystem::blockFree(uint16_t bno) {

}

void FileSystem::read(uint16_t bno, uint8_t offset, char *buf, uint8_t sz) {

}

void FileSystem::write(uint16_t bno, uint8_t offset, char *buf, uint8_t sz) {

}
