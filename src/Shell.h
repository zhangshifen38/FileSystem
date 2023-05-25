//
// Created by AlexHoring on 2023/5/25.
//

#ifndef FILESYSTEM_SHELL_H
#define FILESYSTEM_SHELL_H

#include <string>
#include <iostream>
#include "sstream"
#include "vector"
/*
 * @brief 一个命令行界面，实现类似Linux的命令行交互
 */
class Shell {
private:
    std::vector<std::string> cmd;//用户输入的整行命令
    std::string user="user";//当前登录用户名
public:
    Shell();
    //界面主程序
    [[noreturn]] void running_shell();
    //cd命令处理程序
    void cmd_cd();
    //ls命令处理程序
    void cmd_ls();
    //mkdir命令处理程序
    void cmd_mkdir();
    //rmdir命令处理程序
    void cmd_rmdir();
    //touch命令处理程序
    void cmd_touch();
    //rm命令处理程序
    void cmd_rm();
    //cp命令处理程序
    void cmd_cp();
    //mv命令处理程序
    void cmd_mv();

    const std::vector<std::string> &getCmd() const;

    void setCmd(const std::vector<std::string> &cmd);

    [[nodiscard]] const std::string &getUser() const;

    void setUser(const std::string &user);
    void outPutPrefix();

};


#endif //FILESYSTEM_SHELL_H
