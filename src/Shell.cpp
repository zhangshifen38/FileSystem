//
// Created by AlexHoring on 2023/5/25.
//

#include "Shell.h"


void Shell::running_shell() {
    std::string input;
    std::string word;
    while(true){
        outPutPrefix();
        std::getline(std::cin,input);
        cmd.clear();
        std::istringstream ss(input);
        while(ss>>word){
            cmd.push_back(word);
        }
//        std::string cmd_1=words[0];
//        if(cmd_1=="cd"){
//            cmd_cd();
//        }
//        else if(cmd_1=="ls"){
//            cmd_ls();
//        }else if(cmd_1=="mkdir"){
//            cmd_mkdir();
//        }else if(cmd_1=="rmdir"){
//            cmd_rmdir();
//        }else if(cmd_1=="touch"){
//            cmd_touch();
//        }else if(cmd_1=="rm"){
//            cmd_rm();
//        }else if(cmd_1=="cp"){
//            cmd_cp();
//        }else if(cmd_1=="mv"){
//            cmd_mv();
//        }else{
//            std::cout<<"undefined cmd!"<<std::endl;
//        }
        for(const std::string& w:cmd){
            std::cout<<w<<std::endl;
        }
    }

}



const std::string &Shell::getUser() const {
    return user;
}

void Shell::setUser(const std::string &user) {
    Shell::user = user;
}

Shell::Shell() {
    running_shell();
}

void Shell::outPutPrefix() {
    std::cout<<"FileSystem@"<<getUser()<<":~$";
}

void Shell::cmd_cd() {
    std::cout<<"cd"<<std::endl;
    //进入用户主目录处理
    if(cmd.size()==1){

    }
    else{
        //进入根目录处理
        if(cmd[1]=="/"){
            std::cout<<cmd[1]<<std::endl;
        }
        //进入用户主目录处理
        else if(cmd[1]=="~"){
            std::cout<<cmd[1]<<std::endl;
        }
        //返回上级目录处理
        else if(cmd[1]=="../"||cmd[1]==".."){
            std::cout<<cmd[1]<<std::endl;
        }
        //返回上两级目录处理
        else if(cmd[1]=="../.."){
            std::cout<<cmd[1]<<std::endl;
        }
    }
}

const std::vector<std::string> &Shell::getCmd() const {
    return cmd;
}

void Shell::setCmd(const std::vector<std::string> &cmd) {
    Shell::cmd = cmd;
}

void Shell::cmd_ls() {
    std::cout<<"ls"<<std::endl;
    //显示当前文件夹一般文件处理
    if(cmd.size()==1){

    }
    //显示所有文件处理
    if(cmd[1]=="-a"){
        for(auto c:cmd){
            std::cout<<c<<std::endl;
        }
    }else if(cmd[1]=="-l"){
        //详细信息显示处理
        for(auto c:cmd){
            std::cout<<c<<std::endl;
        }
    }
    else if(cmd[1]=="-h"){
        //人性化显示，文件大小以B、K、M显示
        for(auto c:cmd){
            std::cout<<c<<std::endl;
        }
    }
    else if(cmd[1]=="-i"){
        //显示Linux对于每个文件的id
        for(auto c:cmd){
            std::cout<<c<<std::endl;
        }
    }
    else if(cmd[1]=="-t"){
        //按时间信息排序
        for(auto c:cmd){
            std::cout<<c<<std::endl;
        }
    }
    else if(cmd[1]=="-d"){
        //显示目录属性
        for(auto c:cmd){
            std::cout<<c<<std::endl;
        }
    }
}

void Shell::cmd_mkdir() {
    std::cout<<"mkdir"<<std::endl;
    //创建文件夹
    if(cmd.size()==2){
        for(auto c:cmd){
            std::cout<<c<<std::endl;
        }
    }
    else{
        //建立目录的同时设置目录的权限
        if(cmd[1]=="-m"){
            for(auto c:cmd){
                std::cout<<c<<std::endl;
            }
        }
        //若所要建立目录的上层目录目前尚未建立，则会一并建立上层目录
        else if(cmd[1]=="-p"){
            for(auto c:cmd){
                std::cout<<c<<std::endl;
            }
        }
    }
}

void Shell::cmd_rmdir() {
    std::cout<<"rmdir"<<std::endl;
    //删除文件夹
    if(cmd.size()==2){
        for(auto c:cmd){
            std::cout<<c<<std::endl;
        }
    }
    else{
        //建立目录的同时设置目录的权限
        if(cmd[1]=="-m"){
            for(auto c:cmd){
                std::cout<<c<<std::endl;
            }
        }
            //若所要建立目录的上层目录目前尚未建立，则会一并建立上层目录
        else if(cmd[1]=="-p"){
            for(auto c:cmd){
                std::cout<<c<<std::endl;
            }
        }
    }
}
