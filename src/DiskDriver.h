//
// Created by AlexHoring on 2023/5/23.
//

#ifndef FILESYSTEM_DISKDRIVER_H
#define FILESYSTEM_DISKDRIVER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

/*
 * @brief: 模拟磁盘，支持挂载磁盘模拟文件、读写头前后移动（以字节为单位）、初始化磁盘功能
 * @author: AlexHoring
 */
class DiskDriver {
public:
    static DiskDriver* getInstance();       //为了防止冲突，使用单例获取虚拟磁盘对象
    static void revokeInstance();           //销毁单例
    bool open();                            //打开虚拟磁盘文件，返回是否打开成功
    bool close();                           //关闭虚拟磁盘文件，返回是否关闭
    bool init(uint32_t sz);                 //创建未格式化的指定容量的虚拟磁盘文件，单位为Byte
    void seekStart(uint32_t sz);            //将读写头移动到距起始sz字节处
    void seekCurrent(uint32_t sz);          //将读写头移动到距当前位置sz字节处
    void read(char* buf, uint32_t sz);      //从当前位置读出sz字节到buf缓冲区
    void write(const char *buf, uint32_t sz);     //从当前位置将sz字节写入文件
    ~DiskDriver();
private:
    static DiskDriver *instance;
    static std::string diskName;        //虚拟磁盘文件名
    std::fstream disk;                  //C++文件对象模拟磁盘，同时起到读写头的作用
    bool isOpen;                        //磁盘是否打开标记
    DiskDriver();
};


#endif //FILESYSTEM_DISKDRIVER_H
