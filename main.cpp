#include <iostream>

#include "SystemClass.h"
#include "Settings.h"

Settings SETTINGS;

int main() {

    SystemClass *System = new SystemClass;

    try {
        System->Initialize();
        System->Run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        System->Shutdown();
        delete System;
        System = nullptr;
        return EXIT_FAILURE;
    }

    System->Shutdown();
    delete System;
    System = nullptr;

    return EXIT_SUCCESS;

}