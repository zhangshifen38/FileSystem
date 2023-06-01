//
// Created by AlexHoring on 2023/5/25.
//

#include "Shell.h"


void Shell::running_shell() {
    init();
    std::string input;
    std::string word;

    while(!isExit){
        if(user.uid==0){
            cmd_login();
            std::getchar();
            nowPath.clear();
        }
        userInterface->updateDirNow();
        outPutPrefix();
        std::getline(std::cin,input);
        cmd.clear();
        std::istringstream ss(input);
        while(ss>>word){
            cmd.push_back(word);
        }
        std::string cmd_1=cmd[0];
        if(cmd_1=="cd"){
            cmd_cd();
            continue;
        }else if(cmd_1=="mkdir"){
            cmd_mkdir();
            continue;
        }
        else if(cmd_1=="ls") {
            cmd_ls();
            continue;
        }
        else if(cmd_1=="format") {
            cmd_format();
            continue;
        }else if(cmd_1=="rmdir"){
            cmd_rmdir();
            continue;
        }else if(cmd_1=="touch"){
            cmd_touch();
            continue;
        }else if(cmd_1=="rm") {
            cmd_rm();
            continue;
        }else if(cmd_1=="mv"){
            cmd_mv();
            continue;
        }else if(cmd_1=="chmod"){
            cmd_chmod();
            continue;
        }else if(cmd_1=="rename"){
            cmd_rename();
            continue;
        }else if(cmd_1=="logout"){
            cmd_logout();
        }else if(cmd_1=="open"){
            cmd_open();
            continue;
        }else if(cmd_1=="close"){
            cmd_close();
            continue;
        } else if (cmd_1 == "read") {
            cmd_read();
            continue;
        } else if (cmd_1 == "write") {
            cmd_write();
            continue;
        } else if (cmd_1 == "seek") {
            cmd_seek();
        } else {
            std::cout << "undefined command!" << std::endl;
            continue;
        }
//            std::cout<<"undefined cmd!"<<std::endl;
//        }
//        for(const std::string& w:cmd){
//            std::cout<<w<<std::endl;
//        }
    }
    cout << "Bye!" << endl;
}

Shell::Shell() {
    userInterface = UserInterface::getInstance();
    user.uid = 0;
    isExit = false;
}

void Shell::outPutPrefix() {
    std::cout << "FileSystem@" << user.name << ":";
    for (const auto &s: nowPath) {
        std::cout << "/" << s;
    }
    std::cout << "/ $";
}

void Shell::cmd_cd() {
    if (cmd.size() == 1) {
        return;
    }
    std::string cmd_src = cmd[1];
    vector<string> src = split_path(cmd_src);
    bool ok = true;
    for (string &item: src) {
        if (item == "") {
            userInterface->goToRoot();
            nowPath.clear();
        } else if (item == ".") {
            continue;
        } else if (item == "..") {
            ok = userInterface->cd(item);
            if (ok) {
                if (!nowPath.empty()) {
                    nowPath.pop_back();
                }
            } else {
                break;
            }
        } else {
            ok = userInterface->cd(item);
            if (ok) {
                nowPath.push_back(item);
            } else {
                break;
            }
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
    if (cmd.size() == 1) {
        userInterface->ls();
        return;
    }
    std::string cmd_src = cmd[1];
    //std::vector<std::string> src = splitWithStl(cmd_src,"/");
    vector<string> src = split_path(cmd_src);
    userInterface->ls(src);
}

void Shell::cmd_mkdir() {
    if (cmd.size() <= 1) {
        cout << "mkdir: missing operand" << endl;
        return;
    }
    std::string cmd_src = cmd[1];
    //std::vector<std::string> src = splitWithStl(cmd_src,"/");
    std::vector<std::string> src = split_path(cmd_src);
    if (src.empty()) {
        cout << "mkdir: missing operand" << endl;
        return;
    }
    std::string dirName = src.back();
    src.pop_back();
    if (!src.empty()) userInterface->mkdir(user.uid, src, dirName);
    else userInterface->mkdir(user.uid, dirName);
}

void Shell::cmd_rmdir() {
    if (cmd.size() <= 1) {
        cout << "rmdir: missing operand" << endl;
        return;
    }
    std::string cmd_src = cmd[1];
    //std::vector<std::string> src = splitWithStl(cmd_src,"/");
    std::vector<std::string> src = split_path(cmd_src);
    if (src.empty()) {
        cout << "rmdir: missing operand" << endl;
        return;
    }
    std::string dirName = src.back();
    src.pop_back();
    if (!src.empty()) userInterface->rmdir(user.uid, src, dirName);
    else userInterface->rmdir(user.uid, dirName);
}

void Shell::cmd_rm() {
    if (cmd.size() <= 1) {
        cout << "rm: missing operand" << endl;
        return;
    }
    std::string cmd_src = cmd[1];
    //std::vector<std::string> src = splitWithStl(cmd_src,"/");
    std::vector<std::string> src = split_path(cmd_src);
    if (src.empty()) {
        cout << "rm: missing operand" << endl;
        return;
    }
    std::string fileName = src.back();
    src.pop_back();
    if (!src.empty()) userInterface->rm(user.uid, src, fileName);
    else userInterface->rm(user.uid, fileName);
}

void Shell::init() {
    cout << "               +-------------------------------------+" << endl;
    cout << "               |    Simple FileSystem Simulation     |" << endl;
    cout << "               +-------------------------------------+" << endl;
    userInterface->initialize();

}

void Shell::cmd_login() {
    string userName;
    string password;
    while (1) {
        cout << "Login as: " << flush;
        cin >> userName;
        cout << "Password: " << flush;
        cin >> password;

        uint8_t uid = userInterface->userVerify(userName, password);
        if (uid == 0) {
            cout << "Access denied.Please check username or password.\n" << endl;
        } else {
            userInterface->getUser(uid, &user);
            break;
        }
    }
}

std::vector<std::string> Shell::splitWithStl(std::string str, std::string part) {
    std::vector<std::string> resVec;

    if ("" == str)//若为空，则返回原字符串
    {
        return resVec;
    }
    //方便截取最后一段数据
    std::string strs = str + part;

    size_t pos = strs.find(part);//find函数的返回值，若找到分隔符返回分隔符第一次出现的位置，
    //否则返回npos
    //此处用size_t类型是为了返回位置
    size_t size = strs.size();

    while (pos != std::string::npos) {
        std::string x = strs.substr(0, pos);//substr函数，获得子字符串
        resVec.push_back(x);
        strs = strs.substr(pos + 1, size);
        pos = strs.find(part);
    }
    return resVec;
}

void Shell::cmd_touch() {
    if (cmd.size() <= 1) {
        cout << "touch: missing operand" << endl;
        return;
    }
    std::string cmd_src = cmd[1];
    //std::vector<std::string> src = splitWithStl(cmd_src,"/");
    vector<string> src = split_path(cmd_src);
    if (src.empty()) {
        cout << "touch: missing operand" << endl;
        return;
    }
    std::string fileName = src.back();
    src.pop_back();
    if (!src.empty()) userInterface->touch(user.uid, src, fileName);
    else userInterface->touch(user.uid, fileName);
}

void Shell::cmd_mv() {
    if (cmd.size() != 3) {
        cout << "mv: missing operand" << endl;
        return;
    }
    std::string cmd_src = cmd[1];
    std::vector<std::string> src = split_path(cmd_src);
    if (src.empty()) {
        cout << "mv: missing operand" << endl;
        return;
    }
    std::string cmd_des = cmd[2];
    std::vector<std::string> des = split_path(cmd_des);
    if (des.empty()) {
        cout << "mv: missing operand" << endl;
        return;
    }
    userInterface->mv(src,des);
}

void Shell::cmd_rename() {
    if (cmd.size() != 3) {
        cout << "rename: missing operand" << endl;
        return;
    }
    std::string cmd_src = cmd[1];
    std::vector<std::string> src = split_path(cmd_src);
    if (src.empty()) {
        cout << "rename: missing operand" << endl;
        return;
    }
    userInterface->rename(src,cmd[2]);
}

void Shell::cmd_format() {
    userInterface->format();
    nowPath.clear();
}

void Shell::cmd_chmod() {
    if (cmd.size() != 4) {
        cout << "chmod: missing operand" << endl;
        return;
    }
    std::string cmd_src = cmd[3];
    std::vector<std::string> src = split_path(cmd_src);
    if (src.empty()) {
        cout << "chmod: missing operand" << endl;
        return;
    }
    userInterface->chmod(cmd[1], cmd[2], src);
}

void Shell::cmd_logout() {
    userInterface->logOut();
    string op = "-s";
    if (cmd.size() == 2) {
        op = cmd[1];
    } else if (cmd.size() != 1) {
        cout << "logout: too much operand" << endl;
        return;
    }
    if (op != "-s" && op != "-e") {
        cout << "logout: unknown oprand: \'" << op << "\'" << endl;
        return;
    }
    user.uid = 0;
    if (op == "-e") {
        isExit = true;
    }
}

void Shell::cmd_zedit() {

}

vector<string> Shell::split_path(string &path) {
    vector<string> paths;
    path += '/';      //方便程序处理
    string item = "";
    bool occur = false;    //判断是否出现路径符号/，多个斜杠算作一个
    for (char ch: path) {
        if (ch == '/') {
            if (!occur) {
                paths.push_back(item);
                item = "";
                occur = true;
            } else {
                continue;
            }
        } else {
            occur = false;
            item += ch;
        }
    }
    return paths;
}


void Shell::cmd_open() {
    if (cmd.size() != 3) {
        cout << "open: missing operand" << endl;
        return;
    }
    std::string cmd_src = cmd[2];
    std::vector<std::string> src = split_path(cmd_src);
    if (src.empty()) {
        cout << "open: missing operand" << endl;
        return;
    }
    userInterface->open(cmd[1], src);
}

void Shell::cmd_close() {
    if (cmd.size() != 2) {
        cout << "close: missing operand" << endl;
        return;
    }
    std::string cmd_src = cmd[1];
    std::vector<std::string> src = split_path(cmd_src);
    if (src.empty()) {
        cout << "close: missing operand" << endl;
        return;
    }
    userInterface->close(src);
}

void Shell::cmd_read() {
    if(cmd.size()<3){
        cout<<"read: missing operand"<<endl;
        return;
    }
    vector<string> src= split_path(cmd[1]);
    std::stringstream sio;
    sio<<cmd[2];
    uint32_t offset;
    sio>>offset;
    char* buf=new char[offset+1];
    buf[offset]='\0';
    userInterface->read(user.uid,src,buf,offset);
    cout<<"read: "<<endl<<buf<<endl;
    delete[] buf;
}

void Shell::cmd_write() {
    if(cmd.size()<3){
        cout<<"write: missing operand"<<endl;
        return;
    }
    vector<string> src= split_path(cmd[1]);
    string text=cmd[2];
    userInterface->write(user.uid,src,text.c_str(),text.length());
}

void Shell::cmd_seek() {
    if(cmd.size()<4){
        cout<<"seek: missing operand"<<endl;
        return;
    }
    vector<string> src= split_path(cmd[1]);
    string option=cmd[2];
    std::stringstream sio;
    sio<<cmd[3];
    uint32_t offset;
    sio>>offset;
    int op=-1;
    if(option=="-b"){
        op=2;
    }else if(option=="-c"){
        op=1;
    }
    if(op==-1){
        cout<<"seek: unknown option: \'"<<option<<"\'"<<endl;
        return;
    }
    userInterface->setCursor(op,src,offset);
}

Shell::~Shell() {
    userInterface->revokeInstance();
}



