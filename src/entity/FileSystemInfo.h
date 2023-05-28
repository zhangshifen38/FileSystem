//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_FILESYSTEMINFO_H
#define FILESYSTEM_FILESYSTEMINFO_H

#include <cstdint>

/*
 * @brief 超级块对象
 */
class FileSystemInfo {
public:
    uint32_t rootLocation;              //根目录所在磁盘块

    uint32_t freeBlockNumber;           //空闲块个数
    uint32_t freeBlockStackTop;         //空闲块栈的栈顶（栈底根据块大小和磁盘大小可以计算）
    uint16_t freeBlockStackOffset;      //空闲块栈栈顶指针所在的块内偏移

    uint32_t avaliableCapasity;         //磁盘可用容量

    uint8_t flag;                       //超级块修改标记
};


#endif //FILESYSTEM_FILESYSTEMINFO_H
