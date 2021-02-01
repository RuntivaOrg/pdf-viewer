#include "viewer/layout.h"
#include <cassert>

namespace PdfViewer::Viewer {

Layout::Layout(Params params, LayoutType type)
        : params(params), layout_type(type), current_panel_index(0), current_panel_position(0) {}

const Params& Layout::get_params() const { return params; }


LayoutType Layout::get_layout_type() const { return layout_type; }
    
void Layout::set_layout_type(LayoutType type) { layout_type = type; }

int Layout::get_layout_height(float zoom_level) {

    int total_panel_height = 0;
    uint32_t panels_count = static_cast<uint32_t>(panels.size());
    uint32_t top_bottom_border_height = panels_count * params.get_border_width_px() * 2;
    uint32_t padding_height = (panels_count + 1) * params.get_padding_px();

    for (auto& p : panels) {
        auto r = p.get_panel_rect();
        total_panel_height += r.height;
    }

    return static_cast<int>(static_cast<float>(total_panel_height) * zoom_level) +
           static_cast<int>(top_bottom_border_height + padding_height);
}

const Panel& Layout::get_panel(int index) const { 
    assert(index < (int)panels.size());
    return panels[(size_t)index]; 
}

void Layout::reset_panels(std::vector<Panel> panels) {
    this->panels.clear();

    this->panels.insert(this->panels.end(), panels.begin(), panels.end());
}

// calculate scroll from current panel -- for resizing, zooming and layout events
int Layout::calculate_pos_for_current_panel_info(float zoom_factor) {

    int pos = 0;

    if (static_cast<int>(panels.size() == 0)) {
        return pos;
    }

    assert(current_panel_index < static_cast<int>(panels.size()));

    uint32_t border_width = params.get_border_width_px();
    uint32_t padding_height = params.get_padding_px();

    // loop through all but last panel
    for (size_t i = 0; i < (size_t)current_panel_index; i++) {
        auto r = panels[i].get_panel_rect();
        pos += static_cast<int>((static_cast<float>(r.height) * zoom_factor)) +
               static_cast<int>(padding_height) +
               (static_cast<int>(border_width) * 2);
    }

    // pos += std::max(0, panels[(size_t)current_panel_index].get_panel_rect().height + current_panel_position);
    pos += std::max(0, 0 - current_panel_position);

    return pos;
}

// calculate current panel from scroll - for scrolling events
void Layout::set_current_panel_from_scroll_pos(int scroll_pos, float zoom_level) {

    uint32_t border_width = params.get_border_width_px();
    uint32_t padding_height = params.get_padding_px();

    size_t idx = 0;
    size_t panel_cnt = panels.size();

    while (scroll_pos > 0 && idx < panel_cnt) {
        auto r = panels[idx].get_panel_rect();

        int panel_height = static_cast<int>(padding_height + border_width +
                           static_cast<uint32_t>(static_cast<float>(std::max(0, r.height)) * zoom_level) +
                           border_width);

        if (scroll_pos >= panel_height) {
            scroll_pos -= panel_height;
        } 
        else {
            break;
        }

        idx++;
    }

    current_panel_index = static_cast<int>(idx);
    current_panel_position = -scroll_pos;// -panels[idx].get_panel_rect().height;

}


void Layout::set_current_panel(int index, int position) {
    current_panel_index = index;
    current_panel_position = position;
}

int Layout::get_current_panel_index() const { return current_panel_index; }
int Layout::get_current_panel_position() const { return current_panel_position; }



uint32_t Layout::get_vertical_slot_offset() const {
    return params.get_padding_px() + params.get_border_width_px();
}


// uint32_t get_padded_panel_height

}  // namespace PdfViewer::Viewer