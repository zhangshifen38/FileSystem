//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_SHELL_H
#define FILESYSTEM_SHELL_H

#include <string>
#include <iostream>
#include "UserInterface.h"
#include "entity/User.h"
#include "sstream"
#include "vector"
#include "Tools.h"

using std::string;
using std::cout;
using std::endl;
using std::flush;
using std::cin;
using std::vector;
/*
 * @brief 一个命令行界面，实现类似Linux的命令行交互
 */
class Shell {
private:
    std::vector<std::string> cmd;//用户输入的整行命令
    User user;                  //当前登录用户
    bool isExit;            //是否退出标记
    UserInterface* userInterface;
    std::vector<std::string> nowPath;//当前从根目录开始的路径
public:
    Shell();
    //根据part分割str
    vector<string> split_path(string& path);
    //界面主程序
    void running_shell();
    //cd命令处理程序
    void cmd_cd();
    //ls命令处理程序
    void cmd_ls();
    //mkdir命令处理程序
    void cmd_mkdir();
    //touch命令处理程序
    void cmd_touch();
    //rmdir命令处理程序
    void cmd_rmdir();
    //rm命令处理程序
    void cmd_rm();
    //mv命令处理程序
    void cmd_mv();
    //rename命令处理程序
    void cmd_rename();
    //format命令处理程序
    void cmd_format();
    //chmod命令处理程序
    void cmd_chmod();
    //open命令处理程序
    void cmd_open();
    //close命令处理程序
    void cmd_close();

    //AlexHoring写的部分
    void init();            //命令行初始化
    void cmd_login();       //登录
    void cmd_logout();      //退出
    void cmd_read();        //读取
    void cmd_write();       //写入
    void cmd_seek();        //文件指针修改
    void cmd_zedit();       //简单文本编辑器


    const std::vector<std::string> &getCmd() const;

    void setCmd(const std::vector<std::string> &cmd);

    void outPutPrefix();
    ~Shell();

};


#endif //FILESYSTEM_SHELL_H
