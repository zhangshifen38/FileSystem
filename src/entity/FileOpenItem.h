//
// Created by AlexHoring on 2023/6/1.
//

#ifndef FILESYSTEM_FILEOPENITEM_H
#define FILESYSTEM_FILEOPENITEM_H

#include <cstdint>
#include "../Constraints.h"
#include "INode.h"
/**
 * @brief 用户打开文件表表项
 */
class FileOpenItem {
    char fileName[FILE_NAME_LENGTH];        //文件名
    uint8_t flag;       //标志位，低2位是读写方式00读，01写，11读写，第3位是修改标记，0未修改1已修改
    INode iNode;        //文件i节点
    uint32_t cursor;    //文件指针，指向当前所在位置
};


#endif //FILESYSTEM_FILEOPENITEM_H
