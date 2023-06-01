//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_CONSTRAINTS_H
#define FILESYSTEM_CONSTRAINTS_H

//目录项的大小，128 bit = 16 Byte
#define DIRECTORY_ITEM_SIZE 128
//块大小，4096 Byte
#define BLOCK_SIZE 32768
//文件名最大长度
#define FILE_NAME_LENGTH ((DIRECTORY_ITEM_SIZE - 32) / (8 * sizeof(char)))
//目录项总项数
#define DIRECTORY_NUMS (BLOCK_SIZE/DIRECTORY_ITEM_SIZE)
//文件索引表总项数
#define FILE_INDEX_SIZE (BLOCK_SIZE/32-1)
//用户名和密码的最大长度
#define USERNAME_PASWORD_LENGTH 32
//同时最多打开文件数
#define FILE_OPEN_MAX_NUM 8

#endif //FILESYSTEM_CONSTRAINTS_H
