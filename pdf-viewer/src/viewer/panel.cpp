#include "viewer/panel.h"

#include <limits>
#include <cassert>

namespace PdfViewer::Viewer {


Panel::Panel(Slot slot) { 
	slots.push_back(slot); 
}

Panel::Panel(std::vector<Slot> slots) {
	this->slots.insert(this->slots.end(), slots.begin(), slots.end()); 
}


Rect Panel::get_panel_rect() const {

	assert(slots.size() > 0);

	int left = std::numeric_limits<int>::max();
    int top = std::numeric_limits<int>::max();
    int right = std::numeric_limits<int>::min();
    int bottom = std::numeric_limits<int>::min();

	for (const auto& s : slots) {
        const auto r = s.get_rect();

		if (r.left < left) left = r.left;
        if (r.top < top) top = r.top;

		if (r.left + r.width > right) right = r.left + r.width;
        if (r.top + r.height > bottom) bottom = r.top + r.height;
	}

	Rect r(Point(left, top), Point(right, bottom));
    return r;
}


void Panel::paint_event(Painter& p, uint32_t border_width_px, Color border_color) const {
    for (const auto& s : slots) {
        s.paint_event(p, border_width_px, border_color);
    }
}

}  // namespace PdfViewer::Viewer
