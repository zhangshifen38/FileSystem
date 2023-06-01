#include <iostream>
#include "src/Constraints.h"
#include "src/Shell.h"
#include "src/DiskDriver.h"
#include "src/FileSystem.h"
#include "src/UserInterface.h"

using std::cout;
using std::endl;

int main() {
    Shell shell;
    shell.running_shell();
    return 0;
}
