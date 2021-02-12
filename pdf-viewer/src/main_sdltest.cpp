#include "../include/macros.h"
DISABLE_WARNINGS
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#pragma GCC diagnostic ignored "-Wc++98-compat-pedantic"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

//#include <SDL.h>
// #include "SDL_image.h"
ENABLE_WARNINGS
#include <iostream>

extern "C"
int WinMain(int argc, char** argv) {

    printf("Hello SDL Build\n");
    return 0;
}
