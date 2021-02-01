#include "widget.h"
#include "application.h"
#include "main_viewer_window.h"
//#include "main_window.hpp"
#include "viewer/document.h"

// #include "macros.h"
// DISABLE_WARNINGS
// #include <SDL.h>
// #include "SDL_image.h"
// ENABLE_WARNINGS

int main(int argc, char** argv) {
#ifdef BUILD_WASM
    printf("starting app\n");
#endif

    Application a(argc, argv);

    printf("starting main window\n");
            // MainWindow mainWindow;
    MainViewerWindow mainviewerWindow;

    return a.exec();
}
