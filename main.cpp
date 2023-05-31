#include <iostream>
#include "src/Constraints.h"
#include "src/Shell.h"
#include "src/DiskDriver.h"
#include "src/FileSystem.h"
#include "src/UserInterface.h"

using std::cout;
using std::endl;

int main() {
    UserInterface *userInterface = UserInterface::getInstance();
    userInterface->initialize();
    userInterface->mkdir(1,"dir1");
    userInterface->mkdir(1,"dir2");
    userInterface->ls();
    userInterface->cd("dir1");
    userInterface->touch(1,"file1.1");
    userInterface->touch(1,"file1.2");
    userInterface->ls();
    userInterface->cd("..");
    std::vector<std::string> src{"dir1","file1.1"};
    std::vector<std::string> des{"dir2"};
    userInterface->mv(1,src,des);
    userInterface->cd("dir2");
    userInterface->ls();
    userInterface->cd("..");
    userInterface->cd("dir1");
    userInterface->ls();
    return 0;
}
