//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_DIRECTORYITEM_H
#define FILESYSTEM_DIRECTORYITEM_H

#include <cstdint>
#include "../Constraints.h"

/*
 * @brief 目录项
 */
class DirectoryItem {
public:
    uint32_t inodeIndex;         //本目录项指向的i节点所在的磁盘块号
    char name[(DIRECTORY_ITEM_SIZE - 32) / (8 * sizeof(char))];          //文件名\目录名
};


#endif //FILESYSTEM_DIRECTORYITEM_H
