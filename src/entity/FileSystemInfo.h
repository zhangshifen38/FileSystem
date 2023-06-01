//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_FILESYSTEMINFO_H
#define FILESYSTEM_FILESYSTEMINFO_H

#include <cstdint>
#include "User.h"

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

    User users[8];                      //用户列表，最多为8
    uint8_t trustMatrix[8][8];          //信赖者矩阵，trustMatrix[i][j]=1代表对i而言j可信赖

    uint32_t fileNumber;                //文件总数
    uint32_t directoryNumber;           //目录总数

    uint8_t flag;                       //超级块修改标记
};


#endif //FILESYSTEM_FILESYSTEMINFO_H
