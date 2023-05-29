//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_USERINTERFACE_H
#define FILESYSTEM_USERINTERFACE_H

#include "FileSystem.h"
#include "Constraints.h"
#include "iostream"
#include "cstring"

/*
 * @brief 为用户提供的接口，支持用户常用的功能
 */
class UserInterface {
public:
    static UserInterface* getInstance();    //为了防止冲突，使用单例获取用户接口对象
    void initialize();//初始化
    void mkdir(uint8_t uid,std::string directoryName);//mkdir命令,创建目录
    void ls();//ls命令,显示当前目录所有文件信息
    ~UserInterface();
    void revokeInstance();
private:
    static UserInterface *instance;
    Directory directory;//当前目录
    uint32_t diretoryDisk;//当前目录所在磁盘块号
    FileSystem *fileSystem;
    UserInterface();
};


#endif //FILESYSTEM_USERINTERFACE_H
