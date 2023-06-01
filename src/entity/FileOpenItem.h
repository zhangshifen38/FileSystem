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
public:
    char fileName[FILE_NAME_LENGTH];        //文件名
    uint8_t flag;       //标志位，低2位是读写方式10读，01写，11读写，第3位是修改标记，0未修改1已修改
    uint32_t fileNumber;    //将文件i节点所在磁盘块设置为该文件的文件号，0说明是空文件打开表项
    INode iNode;        //文件i节点
    uint32_t cursor;    //文件指针，指向当前所在位置
};

//open -r file.txt
//close /usr/local/close.txt
//-r 读 -w写 -rw -wr 读写
#endif //FILESYSTEM_FILEOPENITEM_H
