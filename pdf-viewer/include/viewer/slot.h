#ifndef PDFVIEWER_VIEWER_SLOT_H_DEFINED
#define PDFVIEWER_VIEWER_SLOT_H_DEFINED
#pragma once

#include "geometry.h"
#include "../painter.h"
#include "../color.h"

namespace PdfViewer::Viewer {

class Slot {
public:
    Slot(Rect rect, float scale, int page_index);

    Rect get_rect() const;
    uint32_t get_page_index() const;

    void paint_event(Painter& p, uint32_t border_width_px, Color border_color) const;

private:
    Viewport viewport;
    float slot_scale;
    uint32_t pg_index;

    void draw_border(Painter& p, uint32_t border_width_px, Color border_color) const;

    void draw_image(Painter &p, int x, int y, int w, int h) const;
};


}  // namespace PdfViewer::Viewer
#endif   // PDFVIEWER_VIEWER_SLOT_H_DEFINED
