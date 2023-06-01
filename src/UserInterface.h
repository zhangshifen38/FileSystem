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
#include "vector"
#include "entity/FileOpenItem.h"

/*
 * @brief 为用户提供的接口，支持用户常用的功能
 */
class UserInterface {
public:
    static UserInterface *getInstance();    //为了防止冲突，使用单例获取用户接口对象
    void initialize();//初始化
    //zhl:mkdir检查通过
    void mkdir(uint8_t uid, std::string directoryName);//mkdir命令接口,创建目录
    void mkdir(uint8_t uid, std::vector<std::string> src, std::string directoryName);//mkdir命令接口,根据src指出的路径创建目录
    //zhl:ls检查通过
    void ls();//ls命令接口,显示当前目录所有文件信息
    void ls(std::vector<std::string> src);//ls命令接口,src指出的目录的所有文件信息
    //zhl:touch检查通过
    void touch(uint8_t uid, std::string fileName);//touch命令接口,创建文件
    void touch(uint8_t uid, std::vector<std::string> src, std::string fileName);//touch命令接口,根据src路径创建文件
    //zhl:cd检查通过
    bool cd(std::string directoryName);//cd命令接口,进入当前目录的文件夹，返回切换是否成功
    void cd(std::vector<std::string> src);//cd命令接口,根据src提供的路径进入文件夹
    //zhl:rmdir和rm检查通过
    void rm(uint8_t uid, std::string fileName);//rm命令接口,删除文件
    void rm(uint8_t uid, std::vector<std::string> src, std::string fileName);//rm命令接口,根据src路径删除文件
    void rmdir(uint8_t uid, std::string dirName);//rmdir命令接口,删除文件夹
    void rmdir(uint8_t uid, std::vector<std::string> src, std::string dirName);//rmdir命令接口,根据src路径删除文件夹

    void mv(std::vector<std::string> src, std::vector<std::string> des);//mv命令接口,移动文件或者目录
    void rename(std::vector<std::string> src, std::string newName);//rename命令接口,将src路径指向的文件或者目录改名
    void format();//format命令接口,格式化整个文件系统,并把当前目录设置为根目录
    void chmod(std::string who, std::string how, std::vector<std::string> src);//chmod命令接口,对src指出的文件设置who(uoa)的how权限(rwx)
    //格式为chmod oau rwx src
    void open(std::string how,std::vector<std::string> src);//open命令接口,对src指出的文件以how方式打开并设置文件打开表
    void close(std::vector<std::string> src);//close命令接口,关闭src指出的文件并设置文件打开表
    void setCursor(int code,std::vector<std::string> src,uint32_t offset);//移动文件指针,code=1表示根据当前文件指针设置偏移,code=2表示从0开始设置偏移
    void read(uint8_t uid,std::vector<std::string> src, char* buf,uint16_t sz);//将src指出的文件读sz个字节到buf数组中
    void write(uint8_t uid,std::vector<std::string> src, char* buf,uint16_t sz);//将buf数组的数据写入到src指出的文件中
    void updateDirNow();//更新当前目录信息
    void cp(std::vector<std::string> src, std::vector<std::string> des);//cp命令接口,复制文件或者目录

    bool judge(uint32_t disk);//判断i结点指向的是目录还是文件,目录真,文件假
    int judge(std::vector<std::string> src);//判断src指向的是目录还是文件,文件1,目录2

    ~UserInterface();
    void revokeInstance();

    //zhl part
    uint8_t userVerify(std::string &username, std::string &password);        //用户鉴别，鉴别成功返回uid，否则返回0
    void getUser(uint8_t uid, User *user);                    //根据uid提取用户信息
    void goToRoot();        //进入根目录


private:
    static UserInterface *instance;
    Directory directory;//当前目录
    uint32_t nowDiretoryDisk;//当前目录所在磁盘块号
    FileSystem *fileSystem;

    FileOpenItem fileOpenTable[FILE_OPEN_MAX_NUM];     //文件打开表

    //非接口函数设为私有，不让上层调用
    std::pair<uint32_t, int>
    findDisk(std::vector<std::string> src);//从当前目录开始,根据src数组提供的路径,找到对应文件或者目录所在的目录所在的磁盘块号和该文件或者目录的i结点所在的目录项序号
    //第一个为对应文件或者目录所在的目录所在的磁盘块号;第二个为该文件或者目录的i结点所在的目录项序号;code为方式码,1为文件,2为目录
    uint32_t fileIndexBlockFree(uint32_t disk);//回收文件索引表中所有文件的块以及文件索引表本身的块,并返回还有没有下一个索引
    void wholeDirItemsMove(int itemLocation);//将从指定位置开始的目录项整体前移
    bool duplicateDetection(std::string name);//重复名检测


    UserInterface();
};


#endif //FILESYSTEM_USERINTERFACE_H
