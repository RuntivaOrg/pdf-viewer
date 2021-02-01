#ifndef PDFVIEWER_VIEWER_GEOMETRY_H_DEFINED
#define PDFVIEWER_VIEWER_GEOMETRY_H_DEFINED
#pragma once

#include <cstdint>

namespace PdfViewer::Viewer {

enum class LayoutDirection {
    Horizonal,
    Vertical
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow-field-in-constructor"
struct Point {
    int x;
    int y;

    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}
};

struct Size {
    int width;
    int height;
    Size(int width, int height) : width(width), height(height) {}
};
#pragma GCC diagnostic pop

struct Rect {
    int left;
    int top;
    int width;
    int height;

    Rect(int left, int top, int width, int height);
    Rect(int left, int top, const Size& size);
    Rect(const Point& top_left, const Point& bottom_right);
};


class Viewport {
public:
    Viewport(int left, int top, int width, int height);
    Viewport(Rect rect);

    void resize(const Size& size);
    void resize(int width, int height);

    void move(const Point& point);
    void move(int left, int top);

    int left() const;
    int top() const;
    int width() const;
    int height() const;

    int center_x() const;
    int center_y() const;

    Point position() const;
    Size size() const;
    Rect rect() const;

private:
    Rect _rect;
};



}  // namespace PdfViewer::Viewer

#endif  // PDFVIEWER_VIEWER_GEOMETRY_H_DEFINED
