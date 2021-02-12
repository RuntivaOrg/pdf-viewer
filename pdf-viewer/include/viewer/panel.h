#ifndef PDFVIEWER_VIEWER_PANEL_H_DEFINED
#define PDFVIEWER_VIEWER_PANEL_H_DEFINED
#pragma once

#include "geometry.h"
#include "slot.h"
#include <vector>

namespace PdfViewer::Viewer {

// Panel contains a collection of Slots that are laid out horizontally or vertically. 
class Panel {
public:
    Panel();
    Panel(Slot slot);
    Panel(std::vector<Slot> slots);
    Panel(const Panel &other);
    ~Panel();
    
    int get_slot_count() const { return static_cast<int>(slots.size()); }
    Rect get_panel_rect() const; 

    void paint_event(Painter& p, uint32_t border_width_px, Color border_color) const;

private:
    //Rect rect;
    std::vector<Slot> slots;
};

}  // namespace PdfViewer::Viewer
#endif  // PDFVIEWER_VIEWER_PANEL_H_DEFINED
