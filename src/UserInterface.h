//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_USERINTERFACE_H
#define FILESYSTEM_USERINTERFACE_H

#include <stdio.h>






#include "FileSystem.h"
#include "Constraints.h"
#include "iostream"
#include "cstring"
#include "entity/FileIndex.h"
#include "Tools.h"


/*
 * @brief 为用户提供的接口，支持用户常用的功能
 */
class UserInterface {
public:
    static UserInterface* getInstance();    //为了防止冲突，使用单例获取用户接口对象
    void initialize();//初始化
    void mkdir(uint8_t uid,std::string directoryName);//mkdir命令接口,创建目录
    bool judge(uint32_t disk);//判断是目录还是文件,目录真,文件假
    void ls();//ls命令接口,显示当前目录所有文件信息
    void touch(uint8_t uid,std::string fileName);//touch命令接口,创建文件
    bool duplicateDetection(std::string name);//重复名检测
    void cd(std::string directoryName);//cd命令接口,进入文件夹
    uint32_t fileIndexBlockFree(uint32_t disk);//回收文件索引表中所有文件的块以及文件索引表本身的块,并返回还有没有下一个索引
    void rm(uint8_t uid,std::string fileName);//rm命令接口,删除文件
    void rmdir(uint8_t uid,std::string dirName);//rmdir命令接口,删除文件夹
    ~UserInterface();
    void revokeInstance();
private:
    static UserInterface *instance;
    Directory directory;//当前目录
    uint32_t nowDiretoryDisk;//当前目录所在磁盘块号
    FileSystem *fileSystem;
    UserInterface();
};


#endif //FILESYSTEM_USERINTERFACE_H
