#include "viewer/scroll.h"

#include "color.h"
#include "painter.h"
#include <algorithm>

namespace PdfViewer::Viewer {

#define LINE_HEIGHT 25

Scroll::Scroll(Widget* parent, int min_pos, int max_pos, bool enabled, bool visible) : Widget(parent) {
    scroll_info = {{min_pos, max_pos}, 0, 0, 0};
    is_visible = visible;
    is_enabled = enabled;
}

ScrollRange Scroll::get_scroll_range() const { return scroll_info.range; }
int Scroll::get_scroll_pos() const { return scroll_info.pos; }
uint32_t Scroll::get_scroll_pagesize() const { return scroll_info.page_size; }

void Scroll::set_scroll_range(int min_pos, int max_pos, bool redraw) {
    scroll_info.range.min = min_pos;
    scroll_info.range.max = max_pos;

    if (redraw) {
        update();
    }
}

int Scroll::set_scroll_pos(int pos, bool redraw) {
    int current_pos = scroll_info.pos;
    scroll_info.pos = pos;

    if (redraw) {
        update();
    }

    return current_pos;
}

void Scroll::set_scroll_pagesize(uint32_t pagesize, bool redraw) {
    scroll_info.page_size = pagesize;

    if (redraw) {
        update();
    }
}


void Scroll::show_scrollbar(bool show) {
    bool cur_val = is_visible;

    is_visible = show;

    if (cur_val != is_visible) {
        update();
    }
}

void Scroll::enable_scrollbar(bool enable) {
    bool cur_val = is_enabled;

    is_enabled = enable;

    if (cur_val != is_enabled) {
        update();
    }
}

void Scroll::scroll_linedown(bool redraw) { 
    int prev_pos = scroll_info.pos;
    scroll_info.pos = std::min(scroll_info.range.max, scroll_info.pos + LINE_HEIGHT);

    if (scroll_info.pos != prev_pos && redraw) {
        update();
    }
}

void Scroll::scroll_lineup(bool redraw) {
    int prev_pos = scroll_info.pos;
    scroll_info.pos = std::max(scroll_info.range.min, scroll_info.pos - LINE_HEIGHT);

    if (scroll_info.pos != prev_pos && redraw) {
        update();
    }
}

void Scroll::scroll_pagedown(bool redraw) {
    int prev_pos = scroll_info.pos;
    scroll_info.pos = std::min(scroll_info.range.max,
                               scroll_info.pos + static_cast<int>(scroll_info.page_size));

    if (scroll_info.pos != prev_pos && redraw) {
        update();
    }
}

void Scroll::scroll_pageup(bool redraw) {
    int prev_pos = scroll_info.pos;
    scroll_info.pos = std::max(scroll_info.range.min,
                               scroll_info.pos - static_cast<int>(scroll_info.page_size));

    if (scroll_info.pos != prev_pos && redraw) {
        update();
    }
}


//void Scroll::update() {}

// Paints the scrollbar
void Scroll::paintEvent(PaintEvent& /*event*/) { 
    
    // Painter p(this); 

    // int l = 0;
    // int t = 0;
    // int w = 115; //width();
    // int h = 125;  // height();

    // p.setColor(Blue);
    // p.drawRect(l, t, w, h);
}

}  // namespace PdfViewer::Viewer