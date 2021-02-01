#include "viewer/slot.h"
#include "globals.h"

namespace PdfViewer::Viewer {

Slot::Slot(Rect rect, float scale, int page_index) : viewport(rect), slot_scale(scale), pg_index(page_index) {}

Rect Slot::get_rect() const { return viewport.rect(); }
uint32_t Slot::get_page_index() const { return pg_index; }

void Slot::paint_event(Painter& p, uint32_t border_width_px, Color border_color) const {
    draw_border(p, border_width_px, border_color);

    // draw background for now -- needs to be PDF page
    p.setColor(White);
    p.drawRect(viewport.left(), viewport.top(), viewport.width(), viewport.height());
    
    // temp -- draw image
    draw_image(p, viewport.left(), viewport.top(), viewport.width(), viewport.height());
}


void Slot::draw_border(Painter& p, uint32_t border_width_px, Color border_color) const {
    int l = viewport.left() - static_cast<int>(border_width_px);// -1;
    int t = viewport.top() - static_cast<int>(border_width_px);// - 1;
    int r = viewport.left() + viewport.width();// + 1;
    int b = viewport.top() + viewport.height();// + 1;

    p.setColor(border_color);
    // left
    p.drawLine(l, b, l, t);
    // top
    p.drawLine(l, t, r, t);
    // right
    p.drawLine(r, t, r, b);
    // bottom
    p.drawLine(r, b, l, b);
}

void Slot::draw_image(Painter &p, int x, int y, int w, int h) const {

    if (g_document) {
        auto ppm_data = g_document->load_page_ppm(pg_index, slot_scale);
        p.render_image(ppm_data, x, y, w, h);
    }
}

}  // namespace PdfViewer::Viewer
