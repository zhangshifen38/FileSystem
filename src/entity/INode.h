//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_INODE_H
#define FILESYSTEM_INODE_H

#include <cstdint>

class INode {
public:
    uint8_t uid;            //所属用户ID，默认文件创建者就是文件所有者，拥有该文件所有权限
    uint8_t flag;           //高2位00表示文件，01表示目录，10表示软链接，中间3位以rwx格式表示信赖者的访问权限，低3位表示其余用户访问权限
    uint32_t bno;           //该文件所在磁盘块号

};


#endif //FILESYSTEM_INODE_H
