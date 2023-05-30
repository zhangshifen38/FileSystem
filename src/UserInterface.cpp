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
            std::cout << "format failed!please input disk size(MB):";
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
    if(duplicateDetection(directoryName)){
        std::cout<<"mkdir:cannot create directory '"<<directoryName<<"':"<<"File exists"<<std::endl;
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
        std::cout << directory.item[i].name << "\t";
    }
    std::cout<<std::endl;
}

void UserInterface::touch(uint8_t uid,std::string fileName) {
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
    if(duplicateDetection(fileName)){
        std::cout<<"touch:cannot create file '"<<fileName<<"':"<<"File exists"<<std::endl;
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
    strcpy(directory.item[directoryIndex].name,fileName.c_str());
    directory.item[directoryIndex].inodeIndex=fileInodeDisk;

    //将更新后的当前目录信息写入磁盘
    fileSystem->write(nowDiretoryDisk, 0, reinterpret_cast<char *>(&directory), sizeof(directory));
    FileIndex fileIndex{};
    //给文件分配空闲磁盘块
    uint32_t fileDisk = fileSystem->blockAllocate();
    fileIndex.index[0]=fileDisk;
    fileIndex.next=0;

    //把文件索引表写入磁盘
    fileSystem->write(fileIndexDisk,0,reinterpret_cast<char*>(&fileIndex),sizeof (fileIndex));

    fileSystem->update();
}

bool UserInterface::duplicateDetection(std::string name) {
    int dirLocation = -1;
    for(int i=0;i<DIRECTORY_NUMS;++i){
        if(directory.item[i].inodeIndex==0){
            break;
        }
        if(std::strcmp(directory.item[i].name,name.c_str())==0){
            dirLocation=i;
            break;
        }
    }
    if(dirLocation==-1){
        return false;
    }else{
        return true;
    }
}

void UserInterface::cd(std::string directoryName) {
//    for(auto & i : directory.item){
//        if(std::strcmp(i.name,directoryName.c_str())==0){
//            //获取指向需要进入的目录的i结点
//            uint32_t diretoryInodeDisk = i.inodeIndex;
//            INode iNode{};
//            fileSystem->read(diretoryInodeDisk,0,reinterpret_cast<char*>(&iNode),sizeof (iNode));
//            //设置新的当前目录
//            nowDiretoryDisk=iNode.bno;
//            fileSystem->read(iNode.bno,0,reinterpret_cast<char*>(&directory),sizeof (directory));
//            std::cout<<"/"<<directoryName<<std::endl;
//            return;
//        }
//    }
    int dirLocation=-1;
    for(int i=0;i<DIRECTORY_NUMS;i++){
        if(directory.item[i].inodeIndex==0){
            break;
        }
        if(std::strcmp(directory.item[i].name,directoryName.c_str())==0){
            dirLocation=i;
            break;
        }
    }
    if(dirLocation==-1){
        std::cout<<"cd: "<<directoryName<<":No such directory"<<std::endl;
        return;
    }
    //获取指向需要进入的目录的i结点
    uint32_t diretoryInodeDisk = directory.item[dirLocation].inodeIndex;
    INode iNode{};
    fileSystem->read(diretoryInodeDisk,0,reinterpret_cast<char*>(&iNode),sizeof (iNode));
    //设置新的当前目录
    nowDiretoryDisk=iNode.bno;
    fileSystem->read(iNode.bno,0,reinterpret_cast<char*>(&directory),sizeof (directory));
    std::cout<<"/"<<directoryName<<std::endl;
}
