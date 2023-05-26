//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_FILESYSTEM_H
#define FILESYSTEM_FILESYSTEM_H

#include <iostream>
#include <cstdint>
#include "DiskDriver.h"
#include "./entity/FileSystemInfo.h"
#include "./entity/Directory.h"

/*
 * @brief 基本文件系统，实现对于文件的管理
 * @author AlexHoring
 */
class FileSystem {
public:
    static FileSystem* getInstance();
    bool createDisk(uint32_t sz);       //创建一个指定大小的磁盘，单位为Byte
    bool format(uint16_t bsize);        //指定块大小，进行格式化，单位Byte
    bool mount();                       //尝试挂载硬盘，若挂载失败则需要格式化
    uint32_t blockAllocate();           //分配空闲磁盘块
    void blockFree(uint16_t bno);       //回收磁盘块
    void read(uint16_t bno,uint8_t offset,char* buf,uint8_t sz);    //从磁盘块bno偏移offset开始读sz字节到缓冲区buf
    void write(uint16_t bno,uint8_t offset,char* buf,uint8_t sz);   //从磁盘块bno偏移offset开始覆盖写入缓冲区buf开始sz字节
    ~FileSystem();

public:
    static FileSystem* instance;
    DiskDriver* disk;           //虚拟磁盘对象
    uint32_t capasity;          //读取到的磁盘容量
    int8_t isUnformatted;       //未格式化标记，-1未格式化，0已经格式化
    uint16_t blockSize;         //块大小
    bool isOpen;                //文件是否打开标记
    FileSystemInfo systemInfo;  //文件系统超级块
    FileSystem();

};


#endif //FILESYSTEM_FILESYSTEM_H
