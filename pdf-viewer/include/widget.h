#pragma once
#include "macros.h"
#include "paint_device.h"
#include "layoutable.h"
DISABLE_WARNINGS
#include <SDL.h>
ENABLE_WARNINGS
#include <vector>

class KeyEvent;
class TextInputEvent;
class MouseEvent;
struct ResizeEvent;
class PaintEvent;
class Layout;
struct SDL_Window;
struct SDL_Renderer;

class Widget : public PaintDevice, public Layoutable {
    friend class Application;

public:
    Widget(Widget* parent = nullptr);
    virtual ~Widget() override;

    // Size & Position
    virtual void resize(int width, int height) override;
    int width() const override;
    void setWidth(int);
    int height() const override;
    void setHeight(int);
    int left() const;
    virtual void setLeft(int) override;
    int top() const;
    virtual void setTop(int) override;

    // Hierarchy
    Uint32 windowId() const;
    Widget* parent() const;
    Widget* ancestor();
    const std::vector<Widget*>& children() const;


    // Focus
    void setFocus();
    void clearFocus();
    bool hasFocus();

    // Rendering
    virtual SDL_Renderer* renderer() override;

    virtual int gLeft() const;
    virtual int gTop() const;
    void update();
    void setLayout(Layout*);
    Layout* layout();
    const Layout* layout() const;
    virtual int maxHeight() const override;
    virtual int minHeight() const override;
    virtual int maxWidth() const override;
    virtual int minWidth() const override;

protected:
    // some of following events handlers return bool, true means event handled and does not require
    // handling from the parent object
    virtual bool keyPressEvent(KeyEvent&);
    virtual bool keyReleaseEvent(KeyEvent&);
    virtual bool textInputEvent(TextInputEvent&);
    virtual bool mouseDoubleClickEvent(MouseEvent&);
    virtual bool mouseMoveEvent(MouseEvent&);
    virtual bool mousePressEvent(MouseEvent&);
    virtual bool mouseReleaseEvent(MouseEvent&);
    virtual void paintEvent(PaintEvent&);
    virtual void resizeEvent(ResizeEvent&);

private:
    SDL_Window* window_;
    Widget* parent_;
    std::vector<Widget*> children_;
    SDL_Renderer* renderer_;
    SDL_Texture* texture_;
    int width_;
    int height_;
    int left_;
    int top_;
    bool needRepaint_;
    Layout* layout_;
    void addChild(Widget*);
    void removeChild(Widget*);
    void internalPaint(PaintEvent&);
    void updateWithoutRedraw();
    Widget(const Widget&);
    Widget& operator=(const Widget&);
    bool needRepaint() const;
};
