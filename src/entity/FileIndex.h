//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_FILEINDEX_H
#define FILESYSTEM_FILEINDEX_H

#include <cstdint>
#include "../Constraints.h"

/*
 * @brief 文件索引表
 */
class FileIndex {
public:
    uint32_t index[BLOCK_SIZE/32-1];    //数据存放的磁盘块号
    uint32_t next;                      //下一个索引的磁盘块号，支持大文件,没有为0
};


#endif //FILESYSTEM_FILEINDEX_H
