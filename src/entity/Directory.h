//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_DIRECTORY_H
#define FILESYSTEM_DIRECTORY_H

#include "DirectoryItem.h"

/*
 * @brief 目录类，一个目录恰好是一个块大小
 */
class Directory {
public:
    DirectoryItem item[BLOCK_SIZE/DIRECTORY_ITEM_SIZE];     // 32768 / 128 = 256
};


#endif //FILESYSTEM_DIRECTORY_H
