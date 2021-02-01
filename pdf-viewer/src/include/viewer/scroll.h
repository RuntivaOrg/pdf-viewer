#ifndef PDFVIEWER_VIEWER_SCROLL_H_DEFINED
#define PDFVIEWER_VIEWER_SCROLL_H_DEFINED
#pragma once

#include "../widget.h"
#include <cstdint>
//#include "../color.h"

namespace PdfViewer::Viewer {

// Data elements for Scroll object
struct ScrollRange {
    int min;
    int max;
};

struct ScrollInfo {
    // uint32_t flags;
    ScrollRange range;
    uint32_t page_size;
    int pos;
    int track_pos;
};

class Scroll : public Widget {
public:
    Scroll(Widget *parent, int min_pos, int max_pos, bool enabled, bool visible);

    void paintEvent(PaintEvent&) override;

    ScrollRange get_scroll_range() const;
    int get_scroll_pos() const;
    uint32_t get_scroll_pagesize() const;

    void set_scroll_range(int min_pos, int max_pos, bool redraw = true);
    int set_scroll_pos(int pos, bool redraw = true);
    void set_scroll_pagesize(uint32_t pagesize, bool redraw = true);
    void show_scrollbar(bool show);
    void enable_scrollbar(bool enable);

    void scroll_linedown(bool redraw = true);
    void scroll_lineup(bool redraw = true);
    void scroll_pagedown(bool redraw = true);
    void scroll_pageup(bool redraw = true);

private:
    ScrollInfo scroll_info;
    bool is_visible;
    bool is_enabled;

    // const Color background_color;
    // RECT -- pos & size
};

}  // namespace PdfViewer::Viewer
#endif  // PDFVIEWER_VIEWER_SCROLL_H_DEFINED
