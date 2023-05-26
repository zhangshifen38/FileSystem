//
// Created by AlexHoring on 2023/5/23.
//

#include "DiskDriver.h"

DiskDriver *DiskDriver::instance = nullptr;

std::string DiskDriver::diskName = "./disk.zhl";

DiskDriver *DiskDriver::getInstance() {
    if (!instance) {
        instance = new DiskDriver;
    }
    return instance;
}

DiskDriver::DiskDriver() {
    isOpen = false;
}

bool DiskDriver::open() {
    if (isOpen) {
        return true;
    }
    std::ifstream t(diskName);      //使用读入流来判断文件是否存在
    if (t.is_open()) {
        t.close();
        disk.open(diskName, std::ios::in | std::ios::out|std::ios::binary);
        isOpen = true;
        return true;
    }
    return false;
}

bool DiskDriver::close() {
    if (!isOpen) {
        return true;
    }
    disk.close();
    isOpen = false;
    return true;
}

bool DiskDriver::init(uint32_t sz) {
    if (isOpen) {
        return false;
    }
    std::ofstream c(diskName, std::ios::binary | std::ios::out);
    //填充虚拟磁盘
    c.seekp(0,std::ios::beg);
    for(uint32_t i=0;i<sz;++i){
        c.write("",1);
    }
    c.seekp(0,std::ios::beg);
    c.write(reinterpret_cast<char *>(&sz), sizeof(sz));
    int8_t ok=-1;     //未格式化标记
    c.write(reinterpret_cast<char *>(&ok),sizeof(ok));
    c.close();
    return true;
}

void DiskDriver::seekStart(uint32_t sz) {
    disk.seekg(sz,std::ios::beg);
}

void DiskDriver::seekCurrent(uint32_t sz) {
    disk.seekg(sz,std::ios::cur);
}

void DiskDriver::read(char *buf, uint32_t sz) {
    disk.read(buf,sz);
}

void DiskDriver::write(char *buf, uint32_t sz) {
    disk.write(buf,sz);
}

DiskDriver::~DiskDriver() {
    if(isOpen){
        disk.close();
    }
}

void DiskDriver::revokeInstance() {
    delete instance;
    instance= nullptr;
}


