#include <iostream>
#include "Engine/Debug/ErrorDialog.h"

#include "Engine/Core/Application.h"

int main() {

    Application app;

    try {
        app.Run();
    } catch (const std::exception &e) {
        ShowErrorDialog(EscapeMarkup(e.what()));
        //std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;

}