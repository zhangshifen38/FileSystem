#include <iostream>
#include "src/Constraints.h"
#include "src/Shell.h"
#include "src/DiskDriver.h"
#include "src/FileSystem.h"

int main() {
    Shell shell;
    shell.running_shell();
//    FileSystem* fileSystem=FileSystem::getInstance();
//    fileSystem->createDisk(1024*1024*1024);
//    fileSystem->mount();
//    fileSystem->format(4096);
//    uint32_t s;
//    DiskDriver* driver=DiskDriver::getInstance();
//    driver->seekStart(0);
//    driver->read(reinterpret_cast<char *>(&s),sizeof s);

//    std::cout<<fileSystem->capasity<<std::endl;
//    std::cout<<(int)fileSystem->isUnformatted<<std::endl;
//    std::cout<<fileSystem->systemInfo.freeBlockNumber<<std::endl;
//    std::cout<< fileSystem->systemInfo.freeBlockStackTop <<std::endl;
//    std::cout<< fileSystem->systemInfo.rootLocation <<std::endl;
//    std::cout<< fileSystem->systemInfo.avaliableCapasity <<std::endl;
    return 0;
}
