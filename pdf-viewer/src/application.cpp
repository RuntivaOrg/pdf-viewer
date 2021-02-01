#include "application.h"
#include <SDL_ttf.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include "widget.h"
#include "events/key_event.h"
#include "events/paint_event.h"
#include "events/resize_event.h"
#include "events/text_input_event.h"
#include "to_utf16.h"


#if defined(SK_BUILD_FOR_WASM)
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

DISABLE_WARNINGS
#include "SDL_image.h"
ENABLE_WARNINGS

#include <thread>

Application* Application::_instance = nullptr;
void update_callback(void*);

Application::Application(int&, char**)
        : focus_widget(nullptr), need_update_without_redraw(nullptr), last_update(0) {

    printf("Init App start\n");
    if (_instance != nullptr) {
        printf("ERROR: The program can have only one instance of Application.\n");
        abort();
    }
    _instance = this;

    printf("SDL_Init \n");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        printf("SDL_Init Error: %s[n", std::string(SDL_GetError()).c_str());
        abort();
    }
    
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // *new*

    // load support for the PNG image formats
    // int flags=IMG_INIT_PNG;
    // printf("IMG_Init \n");
    // int initted=IMG_Init(flags);
    // if((initted&flags) != flags) {
    //     printf("IMG_Init: Failed to init required jpg and png support!\n");
    //     printf("IMG_Init: %s\n", IMG_GetError());
    //     // handle error
    // }

    printf("SDL_Init_end \n");
    if (TTF_Init() != 0) {
        printf("TTF_Init error: %s\n", std::string(SDL_GetError()).c_str());
    }
}

Application::~Application() {   
    SDL_Quit();
    _instance = nullptr;
}


void update_callback(void* /*args*/) {
    Application::instance()->update();
    // static_cast<Application*>(app)->update();
}

void Application::resize_app(int width, int height) { 
// TODO: Is the first widget always the right one ??
    widget_list[0]->resize(width, height);
}

int Application::exec() {
    quit = false;


#if defined(SK_BUILD_FOR_WASM)
    int args = 0;
    printf("starting emscripten main loop\n");
	emscripten_set_main_loop_arg(update_callback, &args, 0, 1);

#else

    while (!quit) {
        update();
    }
#endif
    return 0;
}


void Application::update() {

        SDL_Event e;
#if defined(SK_BUILD_FOR_WASM)
    if (SDL_PollEvent(&e)) {
#else
    if (SDL_WaitEvent(&e)) {
#endif

        switch (e.type) {
            case SDL_WINDOWEVENT: {
                Widget* w = widgetByWindowId(e.window.windowID);
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_SHOWN:
                        std::cout << "Window " << e.window.windowID << " shown" << std::endl;
                        break;
                    case SDL_WINDOWEVENT_HIDDEN:
                        std::cout << "Window " << e.window.windowID << " hidden" << std::endl;
                        break;
                    case SDL_WINDOWEVENT_EXPOSED: {
                        need_update_without_redraw = w;
                        break;
                    }
                    case SDL_WINDOWEVENT_MOVED:
                        break;
                    // case SDL_WINDOWEVENT_SIZE_CHANGED:
                    case SDL_WINDOWEVENT_RESIZED: {
                        w->resize(e.window.data1, e.window.data2);
#ifdef __APPLE__
                        SDL_RenderPresent(w->renderer_);  // hack for MacOS X
#endif
                        break;
                    }
                    case SDL_WINDOWEVENT_MINIMIZED:
                        std::cout << "Window " << e.window.windowID << " minimized" << std::endl;
                        break;
                    case SDL_WINDOWEVENT_MAXIMIZED:
                        std::cout << "Window " << e.window.windowID << " maximized" << std::endl;
                        break;
                    case SDL_WINDOWEVENT_RESTORED:
                        std::cout << "Window " << e.window.windowID << " restored" << std::endl;
                        break;
                    case SDL_WINDOWEVENT_ENTER:
                        std::cout << "Mouse entered window " << e.window.windowID << std::endl;
                        break;
                    case SDL_WINDOWEVENT_LEAVE:
                        std::cout << "Mouse left window " << e.window.windowID << std::endl;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        std::cout << "Window " << e.window.windowID << " gained keyboard focus"
                                  << std::endl;
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        std::cout << "Window " << e.window.windowID << " lost keyboard focus"
                                  << std::endl;
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        std::cout << "Window " << e.window.windowID << " closed" << std::endl;
                        break;
                    default:
                        std::cout << "Window " << e.window.windowID << " got unknown event "
                                  << static_cast<int>(e.window.event) << std::endl;
                        break;
                }
                break;
            }
            case SDL_KEYDOWN: {
                KeyEvent ke{static_cast<KeyEvent::Key>(e.key.keysym.sym),
                            static_cast<unsigned int>(SDL_GetModState()),
                            static_cast<bool>(e.key.repeat)};
                auto w = focusWidget();
                if (!w)
                    w = widgetByWindowId(e.key.windowID);
                else if (w->ancestor() != widgetByWindowId(e.key.windowID)) {
                    std::cerr << "Unknown windowID " << e.key.windowID << std::endl;
                    break;
                }
                while (w) {
                    if (w->keyPressEvent(ke)) break;
                    w = w->parent();
                }
                break;
            }
            case SDL_KEYUP: {
                KeyEvent ke{static_cast<KeyEvent::Key>(e.key.keysym.sym),
                            static_cast<unsigned int>(SDL_GetModState()),
                            static_cast<bool>(e.key.repeat)};
                auto w = focusWidget();
                if (!w)
                    w = widgetByWindowId(e.key.windowID);
                else if (w->ancestor() != widgetByWindowId(e.key.windowID)) {
                    std::cerr << "Unknown windowID " << e.key.windowID << std::endl;
                    break;
                }

                while (w) {
                    if (w->keyReleaseEvent(ke)) break;
                    w = w->parent();
                }
                break;
            }
            case SDL_TEXTINPUT: {
                TextInputEvent tie{toUtf16(e.text.text)};
                auto w = focusWidget();
                if (!w)
                    w = widgetByWindowId(e.key.windowID);
                else if (w->ancestor() != widgetByWindowId(e.key.windowID)) {
                    std::cerr << "Unknown windowID " << e.key.windowID << std::endl;
                    break;
                }

                while (w) {
                    if (w->textInputEvent(tie)) break;
                    w = w->parent();
                }
                break;
            }
            case SDL_QUIT:
                quit = true;
                break;
        }
    }
    const auto isEmpty = (SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) == 0);
    if (isEmpty || SDL_GetTicks() > last_update + 1000 / 60) {
        //auto x = SDL_GetTicks();
        for (auto w : widget_list)
            if (w->needRepaint()) {
                PaintEvent pe;
                w->internalPaint(pe);
            }
        if (need_update_without_redraw) {
            need_update_without_redraw->updateWithoutRedraw();
            need_update_without_redraw = nullptr;
        }
        last_update = SDL_GetTicks();
        //std::cout << "Update time: " << last_update - x << " "
        //          << (last_update - x > 0 ? 1000 / (last_update - x) : 999) << "fps" << std::endl;
    }

    // Deleting OBject List ????
    //for (auto obj : deleting_object_list) delete obj;
    //deleting_object_list.clear();
}

Application* Application::instance() { return _instance; }

void Application::addWidget(Widget* w) { widget_list.push_back(w); }

void Application::removeWidget(Widget* w) {
    widget_list.erase(std::remove(begin(widget_list), end(widget_list), w), end(widget_list));
}

Widget* Application::widgetByWindowId(Uint32 id) {
    for (const auto w : widget_list)
        if (id == w->windowId()) return w;
    return nullptr;
}

void Application::setFocusWidget(Widget* value) { focus_widget = value; }

Widget* Application::focusWidget() const { return focus_widget; }

void Application::clearFocus() { focus_widget = nullptr; }
