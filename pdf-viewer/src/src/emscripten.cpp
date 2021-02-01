#ifdef BUILD_WASM
#include "application.h"
//#include "emscripten.h"
#include "viewer/document.h"

#include <stdio.h>
//extern Application* Application::_instance;

extern "C" void resize_viewer(int width, int height);
extern "C" void set_options();
extern "C" void quit();

//EMSCRIPTEN_KEEPALIVE
extern "C"
void resize_viewer(int width, int height) { 
    printf("resize_viewer called: (%d x %d)\n", width, height);
    Application::instance()->resize_app(width, height); 
}

//EMSCRIPTEN_KEEPALIVE
extern "C" void set_options() {
    if (g_document) {
        g_document->set_options(*g_options);
    }
}

//EMSCRIPTEN_KEEPALIVE
void quit() {
    if (g_options) {
        delete g_options;
        g_options = nullptr;
    }
    if (g_document) {
        delete g_document;
        g_document = nullptr;
    }


}
#endif
