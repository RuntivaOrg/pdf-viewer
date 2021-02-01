#include "viewer/geometry.h"

namespace PdfViewer::Viewer {


///////////////////////////////////////////////////////////////////////////////////////////////
// Rect Struct

Rect::Rect(int left, int top, int width, int height)
        : left(left), top(top), width(width), height(height) {}

Rect::Rect(int left, int top, const Size& size)
        : left(left), top(top), width(size.width), height(size.height) {}

Rect::Rect(const Point& top_left, const Point& bottom_right)
        : left(top_left.x)
        , top(top_left.y)
        , width(bottom_right.x - top_left.x)
        , height(bottom_right.y - top_left.y) {}


///////////////////////////////////////////////////////////////////////////////////////////////
// Viewport Class

Viewport::Viewport(int left, int top, int width, int height) : _rect(left, top, width, height) {}

Viewport::Viewport(Rect rect) : _rect(rect) {}

void Viewport::resize(const Size& size) { 
    resize(size.width, size.height);
}

void Viewport::resize(int width, int height) {
    _rect.width = width;
    _rect.height = height;
}

void Viewport::move(const Point& point) { 
    move(point.x, point.y); 
}

void Viewport::move(int left, int top) {
    _rect.left = left;
    _rect.top = top;
}

    int Viewport::left() const { return _rect.left; }
int Viewport::top() const { return _rect.top; }
    int Viewport::width() const { return _rect.width; }
int Viewport::height() const { return _rect.height; }

int Viewport::center_x() const {   
    return _rect.width != 0 ? _rect.left + (_rect.width / 2) : _rect.left;
}

int Viewport::center_y() const { 
    return _rect.height != 0 ?  _rect.top + (_rect.height / 2) : _rect.top; 
}

Point Viewport::position() const { return Point(_rect.left, _rect.top); }
Size Viewport::size() const { return Size(_rect.width, _rect.height); }
Rect Viewport::rect() const { return _rect; }


}  // namespace PdfViewer::Viewer
