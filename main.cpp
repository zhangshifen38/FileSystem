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
    userInterface->mkdir(1,"test");
    userInterface->ls();
    return 0;
}
