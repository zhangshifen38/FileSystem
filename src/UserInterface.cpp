//
// Created by AlexHoring on 2023/5/25.
//

#include "UserInterface.h"

UserInterface *UserInterface::instance = nullptr;

UserInterface *UserInterface::getInstance() {
    if (!instance) {
        instance = new UserInterface;
    }
    return instance;
}

UserInterface::UserInterface() {
    fileSystem = FileSystem::getInstance();
}

UserInterface::~UserInterface() {
    FileSystem::revokeInstance();
    UserInterface::revokeInstance();
}

void UserInterface::revokeInstance() {
    delete instance;
    instance = nullptr;
}

void UserInterface::initialize() {
    //如果挂载失败,先格式化
    if (!fileSystem->mount()) {
        std::cout << "mount failed!" << std::endl << "begin format!" << std::endl;
        //如果格式化失败,创建新磁盘
        if (!fileSystem->format(BLOCK_SIZE / 8)) {
            std::cout << "format failed because there is no disk.\nStart creating a disk, please input disk size(MB):"
                      << std::flush;
            uint32_t disk_size;
            std::cin >> disk_size;
            fileSystem->createDisk(disk_size * 1024 * 1024);
            std::cout << "disk create success!" << std::endl;
            fileSystem->mount();
            fileSystem->format(BLOCK_SIZE / 8);
        }
        std::cout << "format success!" << std::endl;
    }
    //读入根节点所在磁盘块
    uint32_t root_disk = fileSystem->getRootLocation();
    //根节点
    INode rootInode{};
    //从根节点所在磁盘块读入根节点信息
    fileSystem->read(root_disk, 0, reinterpret_cast<char *>(&rootInode), sizeof(rootInode));
    //将根目录信息写入当前目录
    fileSystem->read(rootInode.bno, 0, reinterpret_cast<char *> (&directory), sizeof(directory));
    nowDiretoryDisk = rootInode.bno;
}


void UserInterface::mkdir(uint8_t uid, std::string directoryName) {
    int directoryIndex = -1;
    //遍历所有目录项,找到空闲目录项
    for (int i = 0; i < DIRECTORY_NUMS; i++) {
        if (directory.item[i].inodeIndex == 0) {
            directoryIndex = i;
            break;
        }
    }
    //目录项满了
    if (directoryIndex == -1) {
        std::cout << "directory full!" << std::endl;
        return;
    }

    //重复文件检测
    if (duplicateDetection(directoryName)) {
        std::cout << "mkdir:" << YELLOW << "cannot " << RESET << "create directory '" << directoryName << "':"
                  << "File exists" << std::endl;
        return;
    }

    //给新目录的i结点分配空闲磁盘块
    uint32_t directoryInodeDisk = fileSystem->blockAllocate();
    //给新目录的目录项信息分配空闲磁盘块
    uint32_t directoryDisk = fileSystem->blockAllocate();
    Directory newDirectory{};
    //目录的第一项为本目录的信息
    newDirectory.item[0].inodeIndex = directoryInodeDisk;
    //设置本目录./
    strcpy(newDirectory.item[0].name, ".");
    //目录的第二项为上级目录的信息
    newDirectory.item[1].inodeIndex = directory.item[0].inodeIndex;
    //设置上级目录../
    strcpy(newDirectory.item[1].name, "..");
    //设置结束标记
    newDirectory.item[2].inodeIndex = 0;
    //将目录项信息写入磁盘
    fileSystem->write(directoryDisk, 0, reinterpret_cast<char *>(&newDirectory), sizeof(newDirectory));

    //新目录i节点
    INode directoryInode{};

    directoryInode.bno = directoryDisk;
    directoryInode.flag = 0x7f;//01 111 111b
    directoryInode.uid = uid;
    fileSystem->write(directoryInodeDisk, 0, reinterpret_cast<char *>(&directoryInode), sizeof(directoryInode));

    //更新当前目录目录项
    strcpy(directory.item[directoryIndex].name, directoryName.c_str());
    directory.item[directoryIndex].inodeIndex = directoryInodeDisk;

    //将更新后的当前目录信息写入磁盘
    fileSystem->write(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));

    //更新已分配磁盘块
    fileSystem->update();
}


void UserInterface::ls() {
    for (int i = 0; i < DIRECTORY_NUMS && directory.item[i].inodeIndex != 0; i++) {
        if (judge(directory.item[i].inodeIndex)) {
            std::cout << BLUE << directory.item[i].name << RESET << "\t";
        } else std::cout << directory.item[i].name << "\t";
    }
    std::cout << std::endl;
}

void UserInterface::touch(uint8_t uid, std::string fileName) {
    int directoryIndex = -1;
    //遍历所有目录项,找到空闲目录项
    for (int i = 0; i < DIRECTORY_NUMS; i++) {
        if (directory.item[i].inodeIndex == 0) {
            directoryIndex = i;
            break;
        }
    }
    //目录项满了
    if (directoryIndex == -1) {
        std::cout << "directory full!" << std::endl;
        return;
    }

    //重复文件检测
    if (duplicateDetection(fileName)) {
        std::cout << "touch:" << YELLOW << "cannot " << RESET << "create file '" << fileName << "':" << "File exists"
                  << std::endl;
        return;
    }
    //给文件索引表分配空闲磁盘块
    uint32_t fileIndexDisk = fileSystem->blockAllocate();
    //新文件i节点
    INode fileInode{};
    //给新文件的i结点分配空闲磁盘块
    uint32_t fileInodeDisk = fileSystem->blockAllocate();
    fileInode.bno = fileIndexDisk;
    fileInode.flag = 0x3f;//00 111 111b
    fileInode.uid = uid;
    //把i结点写入磁盘
    fileSystem->write(fileInodeDisk, 0, reinterpret_cast<char *>(&fileInode), sizeof(fileInode));
    //更新目录项信息
    strcpy(directory.item[directoryIndex].name, fileName.c_str());
    directory.item[directoryIndex].inodeIndex = fileInodeDisk;

    //将更新后的当前目录信息写入磁盘
    fileSystem->write(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
    FileIndex fileIndex{};
    //给文件分配空闲磁盘块
    uint32_t fileDisk = fileSystem->blockAllocate();
    fileIndex.index[0] = fileDisk;
    fileIndex.index[1] = 0;
    fileIndex.next = 0;

    //把文件索引表写入磁盘
    fileSystem->write(fileIndexDisk, 0, reinterpret_cast<char *>(&fileIndex), sizeof(fileIndex));

    fileSystem->update();
}

bool UserInterface::duplicateDetection(std::string name) {
    int dirLocation = -1;
    for (int i = 0; i < DIRECTORY_NUMS; ++i) {
        if (directory.item[i].inodeIndex == 0) {
            break;
        }
        if (std::strcmp(directory.item[i].name, name.c_str()) == 0) {
            dirLocation = i;
            break;
        }
    }
    if (dirLocation == -1) {
        return false;
    } else {
        return true;
    }
}

bool UserInterface::cd(std::string directoryName) {
    int dirLocation = -1;
    for (int i = 0; i < DIRECTORY_NUMS; i++) {
        if (directory.item[i].inodeIndex == 0) {
            break;
        }
        if (std::strcmp(directory.item[i].name, directoryName.c_str()) == 0 && judge(directory.item[i].inodeIndex)) {
            //确保是目录名
            dirLocation = i;
            break;
        }
    }
    if (dirLocation == -1) {
        std::cout << "cd: " << directoryName << ":No such directory" << std::endl;
        return false;
    }
    //获取指向需要进入的目录的i结点
    uint32_t diretoryInodeDisk = directory.item[dirLocation].inodeIndex;
    INode iNode{};
    fileSystem->read(diretoryInodeDisk, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    //设置新的当前目录
    nowDiretoryDisk = iNode.bno;
    fileSystem->read(iNode.bno, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
    return true;
//    std::cout << "/" << directoryName << std::endl;
}

bool UserInterface::judge(uint32_t disk) {
    INode iNode{};
    fileSystem->read(disk, 0, reinterpret_cast<char *> (&iNode), sizeof(iNode));
    if ((iNode.flag & 0xC0) == 0x40) {//是目录，01,xxx,xxx & 11,000,000 == 01,000,000
        return true;
    } else if ((iNode.flag & 0xC0) == 0) {//是文件，00,xxx,xxx & 11,000,000 == 0
        return false;
    }
    return false;
}

void UserInterface::rm(uint8_t uid, std::string fileName) {
    int fileLocation = -1;
    for (int i = 0; i < DIRECTORY_NUMS; i++) {
        if (directory.item[i].inodeIndex == 0) {
            break;
        }
        if (strcmp(directory.item[i].name, fileName.c_str()) == 0 && !judge(directory.item[i].inodeIndex)) {
            fileLocation = i;
            break;
        }
    }
    if (fileLocation == -1) {
        std::cout << "rm: " << YELLOW << "cannot" << RESET << " remove '" << fileName << "': " << "No such file"
                  << std::endl;
        return;
    }
    //先找到文件索引表,把所有文件和文件索引表的磁盘块回收
    INode fileIndexInode{};
    fileSystem->read(directory.item[fileLocation].inodeIndex, 0, reinterpret_cast<char *>(&fileIndexInode),
                     sizeof(fileIndexInode));
    //循环删除,直到没有下一个索引
    uint32_t next = fileIndexBlockFree(fileIndexInode.bno);
    while (next != 0) next = fileIndexBlockFree(next);
    //回收i结点
    fileSystem->blockFree(directory.item[fileLocation].inodeIndex);
    //更新目录项
    wholeDirItemsMove(fileLocation);

    //将新的目录项写入磁盘
    fileSystem->write(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
    fileSystem->update();
}

uint32_t UserInterface::fileIndexBlockFree(uint32_t disk) {
    FileIndex fileIndex{};
    fileSystem->read(disk, 0, reinterpret_cast<char *>(&fileIndex), sizeof(fileIndex));
    for (int i = 0; i < FILE_INDEX_SIZE && fileIndex.index[i] != 0; i++) {
        //回收文件磁盘块
        fileSystem->blockFree(fileIndex.index[i]);
        fileIndex.index[i] = 0;
    }
    uint32_t next = fileIndex.next;
    fileSystem->blockFree(disk);
    return next;
}

void UserInterface::rmdir(uint8_t uid, std::string dirName) {
    //先查找对应目录
    int dirLocation = -1;
    for (int i = 0; i < DIRECTORY_NUMS; i++) {
        if (directory.item[i].inodeIndex == 0) break;
        if (strcmp(directory.item[i].name, dirName.c_str()) == 0 && judge(directory.item[i].inodeIndex)) {
            dirLocation = i;
            break;
        }
    }
    if (dirLocation == -1) {
        std::cout << "rmdir: " << RED << "failed" << RESET << " to remove '" << dirName << "': " << RED << "No" << RESET
                  << " such directory" << std::endl;
        return;
    }
    //保存当前目录所在磁盘块
    uint32_t nowDisk = nowDiretoryDisk;
    //进入指定目录
    INode dirInode1{};
    fileSystem->read(directory.item[dirLocation].inodeIndex, 0, reinterpret_cast<char *>(&dirInode1),
                     sizeof(dirInode1));
    fileSystem->read(dirInode1.bno, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
    nowDiretoryDisk = dirInode1.bno;

    //一个目录的目录项如果只有前两项或者前一项(根目录),那么该层目录递归结束
    if (directory.item[1].inodeIndex == 0 || directory.item[2].inodeIndex == 0) {
        //回收本层目录所有磁盘块
        fileSystem->blockFree(nowDiretoryDisk);
//        return;
    } else {
        //还有其他项
        //如果是文件,就使用rm接口;如果是目录,就递归调用rmdir
        for (int i = 0; i < DIRECTORY_NUMS; i++) {
            if (directory.item[i].inodeIndex == 0) break;
            //文件
            if (!judge(directory.item[i].inodeIndex)) rm(uid, directory.item[i].name);
                //目录
            else if (strcmp(directory.item[i].name, ".") != 0 && strcmp(directory.item[i].name, "..") != 0) {
                //保存当前目录
                uint32_t nowDiretoryDisk1 = nowDiretoryDisk;
                INode dirInode{};
                fileSystem->read(directory.item[i].inodeIndex, 0, reinterpret_cast<char *>(&dirInode),
                                 sizeof(dirInode));
                //进入下一目录
                fileSystem->read(dirInode.bno, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
                nowDiretoryDisk = dirInode.bno;
                //回收该i结点
                fileSystem->blockFree(directory.item[i].inodeIndex);
                directory.item[i].inodeIndex = 0;
                //递归删除
                rmdir(uid, directory.item[i].name);
                //递归返回时重置当前目录
                fileSystem->read(nowDiretoryDisk1, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
                nowDiretoryDisk = nowDiretoryDisk1;
            }
        }
    }

    //回收指定目录的i结点所在磁盘块
    fileSystem->blockFree(directory.item[dirLocation].inodeIndex);
    directory.item[dirLocation].inodeIndex = 0;
    //重置当前目录
    fileSystem->read(nowDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
    nowDiretoryDisk = nowDisk;
    //更新目录项
    wholeDirItemsMove(dirLocation);
    //将新的目录项写入磁盘
    fileSystem->write(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
    fileSystem->update();
}

void UserInterface::wholeDirItemsMove(int itemLocation) {
    //如果恰好是最后一项
    if (itemLocation == DIRECTORY_NUMS - 1) {
        directory.item[itemLocation].inodeIndex = 0;
        return;
    }
    //目录项整体前移
    for (int i = itemLocation; i < DIRECTORY_NUMS; i++) {
        if (directory.item[i].inodeIndex == 0) break;
        strcpy(directory.item[i].name, directory.item[i + 1].name);
        directory.item[i].inodeIndex = directory.item[i + 1].inodeIndex;
    }
}

void UserInterface::mv(int code, std::vector<std::string> src, std::vector<std::string> des) {
    /*查找源文件或者目录的i结点*/

    //被移动的文件或者目录的i结点所在磁盘块号
    uint32_t srcInodeIndex = 0;
    Directory tmpDirSrc{};
    uint32_t tmpDirDiskSrc;
    int srcIndex;
    switch (code) {
        //移动文件
        case 1: {
            //查找源文件所在的目录所在的磁盘块号以及对应目录项编号
            auto findRes = findDisk(1, src);
            if (findRes.first == -1) {
                std::cout << "mv: " << RED << "failed" << RESET << ":cannot find src" << std::endl;
                return;
            }
            tmpDirDiskSrc = findRes.first;
            fileSystem->read(tmpDirDiskSrc, 0, reinterpret_cast<char *>(&tmpDirSrc), sizeof(tmpDirSrc));
            srcIndex = findRes.second;
            srcInodeIndex = tmpDirSrc.item[srcIndex].inodeIndex;
//            //保存当前目录,设置当前目录为被移动的文件所在的目录
//            std::swap(directory, tmpDir);
//            std::swap(nowDiretoryDisk, tmpDirDisk);
//            //更新被移动的文件所在的目录
//            wholeDirItemsMove(findRes.second);
//            //将新的目录项写入磁盘
//            fileSystem->write(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
//            fileSystem->update();
//            std::swap(directory, tmpDir);
//            std::swap(nowDiretoryDisk, tmpDirDisk);
        }
            break;
            //移动目录
        case 2: {
            //查找源目录所在的目录所在的磁盘块号以及对应目录项编号
            auto findRes = findDisk(2, src);
            if (findRes.first == -1) {
                std::cout << "mv: " << RED << "failed" << RESET << ":cannot find des" << std::endl;
                return;
            }
            tmpDirDiskSrc = findRes.first;
            fileSystem->read(tmpDirDiskSrc, 0, reinterpret_cast<char *>(&tmpDirSrc), sizeof(tmpDirSrc));
            srcIndex = findRes.second;
            srcInodeIndex = tmpDirSrc.item[srcIndex].inodeIndex;
//            //保存当前目录,设置当前目录为被移动的目录所在的目录
//            std::swap(directory, tmpDir);
//            std::swap(nowDiretoryDisk, tmpDirDisk);
//            //更新被移动的目录所在的目录
//            wholeDirItemsMove(findRes.second);
//            //将新的目录项写入磁盘
//            fileSystem->write(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
//            fileSystem->update();
//            std::swap(directory, tmpDir);
//            std::swap(nowDiretoryDisk, tmpDirDisk);
        }
            break;
    }
    if (srcInodeIndex == 0) {
        std::cout << "mv: " << RED << "failed" << RESET << ":cannot find src" << std::endl;
        return;
    }

    /*查找目的目录*/
    //查找目的目录所在的目录所在的磁盘块号以及对应目录项编号
    auto findRes = findDisk(2, des);
    Directory tmpDir{};
    uint32_t tmpDirDisk = findRes.first;
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t desInodeIndex = tmpDir.item[findRes.second].inodeIndex;
    if (desInodeIndex == 0) {
        std::cout << "mv: " << RED << "failed" << RESET << ":cannot find des" << std::endl;
        return;
    }
    INode desInode{};
    fileSystem->read(desInodeIndex, 0, reinterpret_cast<char *>(&desInode), sizeof(desInode));
    tmpDirDisk = desInode.bno;
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    //在目标目录查找空目录项
    int location = -1;
    for (int i = 0; i < DIRECTORY_NUMS; i++) {
        if (tmpDir.item[i].inodeIndex == 0) {
            location = i;
            break;
        }
    }
    if (location == -1) {
        std::cout << "mv: " << RED << "failed" << RESET << ":des directory full" << std::endl;
        return;
    }

    //保存当前目录,设置当前目录为被移动的文件所在的目录
    std::swap(directory, tmpDirSrc);
    std::swap(nowDiretoryDisk, tmpDirDiskSrc);
    //更新被移动的文件所在的目录
    wholeDirItemsMove(srcIndex);
    //将新的目录项写入磁盘
    fileSystem->write(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
    fileSystem->update();
//    std::swap(directory, tmpDirSrc);
    std::swap(nowDiretoryDisk, tmpDirDiskSrc);
    fileSystem->read(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
    strcpy(tmpDir.item[location].name, src.back().c_str());
    tmpDir.item[location].inodeIndex = srcInodeIndex;
    fileSystem->write(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    fileSystem->update();
}

std::pair<uint32_t, int> UserInterface::findDisk(int code, std::vector<std::string> src) {
    bool judgeFileDir;
    if (code == 1) judgeFileDir = false;
    else judgeFileDir = true;

    //获取名
    std::string srcName = src.back();
    src.pop_back();
    uint32_t tmpDirectoryDisk = nowDiretoryDisk;
    bool ok = true;   //能否找到目录
    std::string dirName;   //输出错误信息用
    //在此次直接调用cd函数来寻找
    for (std::string &item: src) {
        if (item == "") {
            //空，直接寻找根目录
            goToRoot();
        } else if (item == "..") {
            //上级
            ok = cd(item);
        } else if (item == ".") {
            //当前
            continue;
        } else {
            ok = cd(item);
        }
        if (!ok) {
            dirName = item;   //记录哪一步出错了
            break;
        }
    }
    if (!ok) {
        std::cout << RED << "failed: " << RESET << "'" << dirName << "' No such directory" << std::endl;
        //还原现场
        nowDiretoryDisk = tmpDirectoryDisk;
        fileSystem->read(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof directory);
        return std::make_pair(-1, -1);
    }
    int location = -1;
    //找到对应i结点
    for (int i = 0; i < DIRECTORY_NUMS; i++) {
        if (directory.item[i].inodeIndex == 0) break;
        if (strcmp(directory.item[i].name, srcName.c_str()) == 0 && (judge(directory.item[i].inodeIndex) ==
                                                                     judgeFileDir)) {
            location = i;
            break;
        }
    }
    if (location == -1) {
        if (judgeFileDir)
            std::cout << RED << "failed " << RESET << "'" << srcName << "' No such directory" << std::endl;
        else std::cout << RED << "failed " << RESET << "'" << srcName << "' No such file" << std::endl;
        //还原现场
        nowDiretoryDisk = tmpDirectoryDisk;
        fileSystem->read(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof directory);
        return std::make_pair(-1, -1);
    }
    //记录下需要返回的数据
    std::pair<uint32_t, int> ret = std::make_pair(nowDiretoryDisk, location);
    //还原现场
    nowDiretoryDisk = tmpDirectoryDisk;
    fileSystem->read(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof directory);
    return ret;
}

void UserInterface::rename(int code, std::vector<std::string> src, std::string newName) {
    auto findRes = findDisk(code, src);
    if (findRes.first == -1) {
        std::cout << "rename: " << RED << "failed" << RESET << ":cannot find src" << std::endl;
        return;
    }
    //找到需要被改名的文件或者目录所在的目录,和其对应的目录项编号
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    int dirLocation = findRes.second;
    strcpy(tmpDir.item[dirLocation].name, newName.c_str());
    fileSystem->write(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
}

uint8_t UserInterface::userVerify(std::string &username, std::string &password) {
    return fileSystem->userVerify(username, password);
}

void UserInterface::getUser(uint8_t uid, User *user) {
    fileSystem->getUser(uid, user);
}

void UserInterface::format() {
    fileSystem->format(BLOCK_SIZE / 8);
    fileSystem->update();
    INode iNode{};
    fileSystem->read(fileSystem->getRootLocation(), 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    nowDiretoryDisk = iNode.bno;
    fileSystem->read(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
}

void UserInterface::chmod(std::string who, std::string how, std::vector<std::string> src) {
    bool hasU = false, hasO = false, hasA = false;
    if (who.find('a') != std::string::npos) hasA = true;
    if (who.find('o') != std::string::npos) hasO = true;
    if (who.find('u') != std::string::npos) hasU = true;
    bool hasR = false, hasW = false, hasX = false;
    if (how.find('r') != std::string::npos) hasR = true;
    if (how.find('w') != std::string::npos) hasW = true;
    if (how.find('x') != std::string::npos) hasX = true;
    uint8_t rwxResult = 0xc0;//11 000 000
    uint8_t a_r = 0xe4;//r-- 11 100 100
    uint8_t a_w = 0xd2;//-w- 11 010 010
    uint8_t a_x = 0xc9;//--r 11 001 001
    uint8_t u_r = 0xE7;//r-- 11 100 111
    uint8_t u_w = 0xD7;//-w- 11 010 111
    uint8_t u_x = 0xCF;//--x 11 001 111
    uint8_t o_r = 0xFC;//r-- 11 111 100
    uint8_t o_w = 0xFA;//-w- 11 111 010
    uint8_t o_x = 0xF9;//--x 11 111 001
    if (hasA) {
        if (hasR) rwxResult |= a_r;
        if (hasW) rwxResult |= a_w;
        if (hasX) rwxResult |= a_x;
    } else {
        if (hasU) {
            if (hasR) rwxResult |= u_r;
            if (hasW) rwxResult |= u_w;
            if (hasX) rwxResult |= u_x;
        }
        if (hasO) {
            if (hasR) rwxResult |= o_r;
            if (hasW) rwxResult |= o_w;
            if (hasX) rwxResult |= o_x;
        }
    }
    auto findRes = findDisk(1, src);
    if (findRes.first == -1) {
        std::cout << "chmod: " << RED << "failed" << RESET << ":no such file" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->write(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    INode iNode{};
    fileSystem->read(tmpDir.item[findRes.second].inodeIndex, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    iNode.flag &= rwxResult;
    fileSystem->write(tmpDir.item[findRes.second].inodeIndex, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
}

void UserInterface::cd(std::vector<std::string> src) {
    auto findRes = findDisk(2, src);
    if (findRes.first == -1) {
        std::cout << "cd: " << RED << "failed" << RESET << ":no such directory" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t InodeDisk = tmpDir.item[findRes.second].inodeIndex;
    INode dirInode{};
    fileSystem->read(InodeDisk, 0, reinterpret_cast<char *>(&dirInode), sizeof(dirInode));
    nowDiretoryDisk = dirInode.bno;
    fileSystem->read(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
}

void UserInterface::mkdir(uint8_t uid, std::vector<std::string> src, std::string dirName) {
    auto findRes = findDisk(2, std::move(src));
    if (findRes.first == -1) {
        std::cout << "mkdir: " << RED << "failed" << RESET << ":no such directory" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t inodeDisk = tmpDir.item[findRes.second].inodeIndex;
    INode iNode{};
    fileSystem->read(inodeDisk, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    tmpDirDisk = iNode.bno;
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
    mkdir(uid, std::move(dirName));
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
}

void UserInterface::ls(std::vector<std::string> src) {
    auto findRes = findDisk(2, src);
    if (findRes.first == -1) {
        std::cout << "ls: " << RED << "failed" << RESET << ":no such directory" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t inodeDisk = tmpDir.item[findRes.second].inodeIndex;
    INode iNode{};
    fileSystem->read(inodeDisk, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    tmpDirDisk = iNode.bno;
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
    ls();
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
}

void UserInterface::touch(uint8_t uid, std::vector<std::string> src, std::string fileName) {
    auto findRes = findDisk(2, src);
    if (findRes.first == -1) {
        std::cout << "touch: " << RED << "failed" << RESET << ":no such directory" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t inodeDisk = tmpDir.item[findRes.second].inodeIndex;
    INode iNode{};
    fileSystem->read(inodeDisk, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    tmpDirDisk = iNode.bno;
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
    touch(uid, fileName);
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
}

void UserInterface::rm(uint8_t uid, std::vector<std::string> src, std::string fileName) {
    auto findRes = findDisk(2, src);
    if (findRes.first == -1) {
        std::cout << "rm: " << RED << "failed" << RESET << ":no such directory" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t inodeDisk = tmpDir.item[findRes.second].inodeIndex;
    INode iNode{};
    fileSystem->read(inodeDisk, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    tmpDirDisk = iNode.bno;
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
    rm(uid, fileName);
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
}

void UserInterface::rmdir(uint8_t uid, std::vector<std::string> src, std::string dirName) {
    auto findRes = findDisk(2, src);
    if (findRes.first == -1) {
        std::cout << "rmdir: " << RED << "failed" << RESET << ":no such directory" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t inodeDisk = tmpDir.item[findRes.second].inodeIndex;
    INode iNode{};
    fileSystem->read(inodeDisk, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    tmpDirDisk = iNode.bno;
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
    rmdir(uid, dirName);
    std::swap(directory, tmpDir);
    std::swap(nowDiretoryDisk, tmpDirDisk);
}

int UserInterface::judge(std::vector<std::string> src) {
    //获取名
    std::string srcName = src.back();
    src.pop_back();
    uint32_t tmpDirectoryDisk = nowDiretoryDisk;
    Directory tmpDirectory = directory;
    while (!src.empty()) {
        std::string dirName = src.front();
        src.erase(src.begin());
        int dirLocation = -1;

        fileSystem->read(tmpDirectoryDisk, 0, reinterpret_cast<char *>(&tmpDirectory), sizeof(tmpDirectory));
        //找到所在的目录
        for (int i = 0; i < DIRECTORY_NUMS; i++) {
            if (tmpDirectory.item[i].inodeIndex == 0) break;
            if (strcmp(tmpDirectory.item[i].name, dirName.c_str()) == 0 && judge(tmpDirectory.item[i].inodeIndex)) {
                dirLocation = i;
                break;
            }
        }
        if (dirLocation == -1) {
            std::cout << RED << "failed " << RESET << "'" << dirName << "' No such directory" << std::endl;
            return 0;
        }
        INode dirInode{};
        fileSystem->read(tmpDirectory.item[dirLocation].inodeIndex, 0, reinterpret_cast<char *>(&dirInode),
                         sizeof(dirInode));
        tmpDirectoryDisk = dirInode.bno;
        fileSystem->read(tmpDirectoryDisk, 0, reinterpret_cast<char *>(&tmpDirectory), sizeof(tmpDirectory));
    }
    int location = -1;
    //找到对应i结点
    for (int i = 0; i < DIRECTORY_NUMS; i++) {
        if (tmpDirectory.item[i].inodeIndex == 0) break;
        if (strcmp(tmpDirectory.item[i].name, srcName.c_str()) == 0) {
            location = i;
            break;
        }
    }
    if (location == -1) {
        std::cout << RED << "failed " << RESET << "'" << srcName << "' No such file or directory" << std::endl;
        return 0;
    }
    INode iNode{};
    fileSystem->read(tmpDirectory.item[location].inodeIndex, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    if (judge(iNode.bno)) return 2;
    else return 1;
}

void UserInterface::goToRoot() {
    INode iNode{};
    fileSystem->read(fileSystem->getRootLocation(), 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    nowDiretoryDisk = iNode.bno;
    fileSystem->read(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
}

void UserInterface::open(std::string how, std::vector<std::string> src) {
    bool hasR = false, hasW = false;
    if (how.find('r') != std::string::npos) hasR = true;
    if (how.find('w') != std::string::npos) hasW = true;
    uint8_t rwResult = 0x00;//000000 00
    uint8_t _r = 0x02;//r 000000 10
    uint8_t _w = 0x01;//w 000000 01
    if (hasR) rwResult |= _r;
    if (hasW) rwResult |= _w;
    auto findRes = findDisk(1, src);
    if (findRes.first == -1) {
        std::cout << "open: " << RED << "failed" << RESET << ":no such file" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t inodeDisk = tmpDir.item[findRes.second].inodeIndex;
    INode iNode{};
    fileSystem->read(inodeDisk, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    std::string fileName = tmpDir.item[findRes.second].name;
    uint32_t fileNumber = inodeDisk;
    //得到文件索引表i结点
    fileSystem->read(tmpDir.item[findRes.second].inodeIndex, 0, reinterpret_cast<char *>(&iNode), sizeof(iNode));
    int fileLocation = -1;
    for (int i = 0; i < FILE_OPEN_MAX_NUM; i++) {
        //已经被打开的文件不能再被打开
        if (fileOpenTable[i].fileNumber == fileNumber) {
            std::cout << "open: " << RED << "failed" << RESET << ":file '" << src.back() << "' already opened"
                      << std::endl;
            return;
        }
        if (fileOpenTable[i].fileNumber == 0) {
            fileLocation = i;
            break;
        }
    }
    if (fileLocation == -1) {
        std::cout << "open: " << RED << "failed" << RESET << ":fileOpenTable full" << std::endl;
        return;
    }
    strcpy(fileOpenTable[fileLocation].fileName, fileName.c_str());
    fileOpenTable[fileLocation].fileNumber = fileNumber;
    fileOpenTable[fileLocation].iNode = iNode;
    fileOpenTable[fileLocation].cursor = 0;
    fileOpenTable[fileLocation].flag = rwResult;
}

void UserInterface::close(std::vector<std::string> src) {
    auto findRes = findDisk(1, src);
    if (findRes.first == -1) {
        std::cout << "close: " << RED << "failed" << RESET << ":no such file" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t inodeDisk = tmpDir.item[findRes.second].inodeIndex;
    uint32_t fileNumber = inodeDisk;
    int fileLocation = -1;
    for (int i = 0; i < FILE_OPEN_MAX_NUM; i++) {
        if (fileOpenTable[i].fileNumber == fileNumber) {
            fileLocation = i;
            break;
        }
    }
    if (fileLocation == -1) {
        std::cout << "close: " << RED << "failed" << RESET << ":no such file opened" << std::endl;
        return;
    }
    fileOpenTable[fileLocation].fileNumber = 0;
}

void UserInterface::setCursor(int code, std::vector<std::string> src, uint32_t offset) {
    auto findRes = findDisk(1, src);
    if (findRes.first == -1) {
        std::cout << "setCursor: " << RED << "failed" << RESET << ":no such file" << std::endl;
        return;
    }
    uint32_t tmpDirDisk = findRes.first;
    Directory tmpDir{};
    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
    uint32_t inodeDisk = tmpDir.item[findRes.second].inodeIndex;
    uint32_t fileNumber = inodeDisk;
    int fileLocation = -1;
    for (int i = 0; i < FILE_OPEN_MAX_NUM; i++) {
        if (fileOpenTable[i].fileNumber == fileNumber) {
            fileLocation = i;
            break;
        }
    }
    if (fileLocation == -1) {
        std::cout << "setCursor: " << RED << "failed" << RESET << ":no such file opened" << std::endl;
        return;
    }
    if (code == 1) {
        fileOpenTable[fileLocation].cursor += offset;
    }
    if (code == 2) {
        fileOpenTable[fileLocation].cursor = offset;
    }
}

//void UserInterface::read(uint8_t uid, std::vector<std::string> src, char *buf, uint16_t sz) {
//    auto findRes = findDisk(1, src);
//    if (findRes.first == -1) {
//        std::cout << "read: " << RED << "failed" << RESET << ":no such file" << std::endl;
//        return;
//    }
//    uint32_t tmpDirDisk = findRes.first;
//    Directory tmpDir{};
//    fileSystem->read(tmpDirDisk, 0, reinterpret_cast<char *>(&tmpDir), sizeof(tmpDir));
//    uint32_t inodeDisk = tmpDir.item[findRes.second].inodeIndex;
//    uint32_t fileNumber = inodeDisk;
//    int fileLocation = -1;
//    for (int i = 0; i < FILE_OPEN_MAX_NUM; i++) {
//        if (fileOpenTable[i].fileNumber == fileNumber) {
//            fileLocation = i;
//            break;
//        }
//    }
//    if (fileLocation == -1) {
//        std::cout << "read: " << RED << "failed" << RESET << ":no such file opened" << std::endl;
//        return;
//    }
//    FileIndex fileIndex{};
//    fileSystem->read(fileOpenTable[fileLocation].iNode.bno,0,reinterpret_cast<char*>(&fileIndex),sizeof (fileIndex));
//    //获取当前光标位置
//    uint32_t nowCursor=fileOpenTable[fileLocation].cursor;
//    //根据当前光标位置计算文件偏移了多少个索引表
//    int fileIndexNums=nowCursor/(FILE_INDEX_SIZE*BLOCK_SIZE_BYTE);
//    //根据当前光标位置计算文件在最后一个索引表中的第一
//    //下一个索引next数
//    int nextNums=totalBlockIndex/BLOCK_SIZE_BYTE;
//    //当前光标不在找到的文件索引表中,根据next找到当前光标所处的文件索引表
//    while(nextNums>0){
//        uint32_t nextBlock= fileIndex.next;
//        fileSystem->read(nextBlock,0,reinterpret_cast<char*>(&fileIndex),sizeof (fileIndex));
//        nextNums--;
//    }
//    //得到光标在索引表中所处的块序号
//    int blockIndex=totalBlockIndex%BLOCK_SIZE_BYTE;
//    uint32_t nowBlock=fileIndex.index[blockIndex];
//    //得到在当前块的偏移量
//    uint16_t offset=nowCursor%BLOCK_SIZE_BYTE;
//    //得到当前块剩余字节
//    uint16_t resBlock=BLOCK_SIZE_BYTE-offset;
//    //得到要读取的字节数共占多少个索引表
//    int fileIndexNums=sz/(BLOCK_SIZE_BYTE*BLOCK_SIZE_BYTE);
//    //得到在最后一个索引表要读取的字节数
//    uint16_t sz_end=sz%(BLOCK_SIZE_BYTE*BLOCK_SIZE_BYTE);
//    //如果要读取超过一个索引表
//    while(fileIndexNums>0){
//
//    }
//    //得到超出当前块的读取字节数
//    uint16_t resByte=sz-resBlock;
//    //已经读了的字节
//    uint16_t readByte=0;
//    if(resByte<=0){
//        fileSystem->read(nowBlock,offset,buf,sz);
//        readByte=sz;
//        return;
//    }
//    //读了超过一个块
//    if(resByte>0){
//        fileSystem->read(nowBlock,offset,buf,resByte);
//        readByte=resByte;
//        int resBlocks=resByte/BLOCK_SIZE_BYTE;
//        int resOffset=resByte%BLOCK_SIZE_BYTE;
//        for(int i=1;i<=resBlock;i++){
//            blockIndex++;
//            fileSystem->read(fileIndex.index[blockIndex],0,buf+readByte,BLOCK_SIZE_BYTE);
//        }
//
//    }
//
//}


