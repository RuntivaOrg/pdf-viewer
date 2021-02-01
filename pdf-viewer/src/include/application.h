#pragma once
#include "macros.h"
//#include "deleting_object.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wlanguage-extension-token"
#pragma GCC diagnostic ignored "-Wc++98-compat-pedantic"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#include <SDL.h>
#pragma GCC diagnostic pop


#include <vector>

class Widget;

class Application {
    friend class Widget;

public:
    Application(int& argc, char** argv);
    ~Application();
    //template <typename T> void queueDelete(T* obj) {
        //deleting_object_list.push_back(new DeletingObject<T>(obj));
    //}
    int exec();
    static Application* instance();

    //static void update_callback();//void* app);
    void update();

    // WASM Only
    void resize_app(int width, int height);

private:
    static Application* _instance;
    std::vector<Widget*> widget_list;
    Widget* focus_widget;
    Widget* need_update_without_redraw;
//    std::vector<BaseDeletingObject*> deleting_object_list;

    Uint32 last_update;
    bool quit = false;

    // Widgets
    void addWidget(Widget*);
    void removeWidget(Widget*);
    Widget* widgetByWindowId(Uint32);
    void setFocusWidget(Widget*);
    Widget* focusWidget() const;
    void clearFocus();
};
