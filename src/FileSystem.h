//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_FILESYSTEM_H
#define FILESYSTEM_FILESYSTEM_H

#include <cstdint>
#include "DiskDriver.h"

/*
 * @brief 基本文件系统，实现对于文件的管理
 * @author AlexHoring
 */
class FileSystem {
public:
    static FileSystem* getInstance();
    bool format(uint16_t bsize);        //指定块大小，进行格式化
    bool mount();                       //尝试挂载硬盘，若挂载失败则需要格式化
    uint16_t blockAllocate();           //分配空闲磁盘块
    void blockFree(uint16_t bno);       //回收磁盘块
    void read(uint16_t bno,uint8_t offset,char* buf,uint8_t sz);    //从磁盘块bno偏移offset开始读sz字节到缓冲区buf
    void write(uint16_t bno,uint8_t offset,char* buf,uint8_t sz);   //从磁盘块bno偏移offset开始覆盖写入缓冲区buf开始sz字节
    ~FileSystem();
private:
    static FileSystem* instance;
    DiskDriver* disk;           //虚拟磁盘对象
    uint32_t capasity;          //读取到的磁盘容量
    int8_t isUnformatted;       //未格式化标记，-1未格式化，0已经格式化
    uint16_t blockSize;         //块大小
    bool isOpen;                //文件是否打开标记

    FileSystem();

};


#endif //FILESYSTEM_FILESYSTEM_H
