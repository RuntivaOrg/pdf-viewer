#ifndef PDFVIEWER_VIEWER_LAYOUT_H_DEFINED
#define PDFVIEWER_VIEWER_LAYOUT_H_DEFINED
#pragma once

#include "panel.h"
#include "params.h"
#include <vector>

namespace PdfViewer::Viewer {

enum class LayoutType {
    SinglePage,
    Continuous,
    Facing,
    ContinuousFacing
};

class Layout {
public:
    Layout(Params params, LayoutType type);

    const Params& get_params() const;

    LayoutType get_layout_type() const;
    void set_layout_type(LayoutType type);

    int get_layout_height(float zoom_level);

    const Panel& get_panel(int index) const;
    void reset_panels(std::vector<Panel> panels);
    int calculate_pos_for_current_panel_info(float zoom_factor);

    void set_current_panel_from_scroll_pos(int scroll_pos, float zoom_level);
    void set_current_panel(int index, int position);
    int get_current_panel_index() const;
    int get_current_panel_position() const;


    uint32_t get_vertical_slot_offset() const;

private:
    Params params;
    LayoutType layout_type;

    // These panels are for scrolling calculations only. See the View::visible_panels class member for panels that are actually used for placement layout rendering.
    // These dimensions also do not include spacing for padding or borders in order to proper calculate zoom dimensions. Slot contents zoom while padding and borders do not.
    std::vector<Panel> panels;

    int current_panel_index;
    int current_panel_position;
};

}  // namespace PdfViewer::Viewer
#endif  // PDFVIEWER_VIEWER_LAYOUT_H_DEFINED
