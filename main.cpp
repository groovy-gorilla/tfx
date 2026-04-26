#include "ErrorDialog.h"
#include "System.h"
#include "Settings.h"
#include <memory>

Settings SETTINGS;

int main() {

    // Inteligentny wskaźnik
    auto system = std::make_unique<System>();


    try {
        system->Initialize();
        system->Run();
    } catch (const std::exception &e) {
        ShowErrorDialog(EscapeMarkup(e.what()));
    }

    return EXIT_SUCCESS;

}