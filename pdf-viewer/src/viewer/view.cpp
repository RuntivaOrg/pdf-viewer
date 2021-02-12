#include "viewer/view.h"
#include "events/key_event.h"
#include "events/resize_event.h"
#include "globals.h"

#include <iostream>
#include <cassert>
#include <chrono>

using std::chrono::nanoseconds;
using std::chrono::duration_cast;

namespace PdfViewer::Viewer {

View::View(Widget* parent, Params params, PanelLayoutType type)
        : Widget(parent)
        , view_layout(params, type)
        , view_viewport(0, 0, 0, 0)
        , scroll(this, 0.f, 0.f, false, false) {
    // Populate the layout Panels/Slots
    PopulateLayout();

    // Recalculate Scroll
    //if (panels.)
    calculate_scroll_range();
    calculate_scroll_pos();
}

View::~View() {

}


///////////////////////////////////////////////////////////////////////////////////////////
// Layout
void View::layout_event(void (View::*layout_func)()) {
    PanelLayoutType prev_type = view_layout.get_layout_type();
    (this->*layout_func)();
    PanelLayoutType new_type = view_layout.get_layout_type();

    if (prev_type == new_type) {
        return;
    }

    int prev_cols = 1;
    int new_cols = 1;

    if (prev_type == PanelLayoutType::Facing || prev_type == PanelLayoutType::ContinuousFacing) {
        prev_cols = 2;
    }
    if (new_type == PanelLayoutType::Facing || new_type == PanelLayoutType::ContinuousFacing) {
        new_cols = 2;
    }

    // Recreate Layout for new Layout Type
    if (prev_cols != new_cols) {
        PopulateLayout();

        // Update current panel info for this layout
        if (new_cols == 1 && prev_cols == 2) {
            view_layout.set_current_panel(view_layout.get_current_panel_index() * 2,
                                          view_layout.get_current_panel_position());
        } else if (new_cols == 2 && prev_cols == 1) {
            view_layout.set_current_panel(view_layout.get_current_panel_index() / 2,
                                          view_layout.get_current_panel_position());
        }
    }

    // Recalculate Scroll
    calculate_scroll_range();
    calculate_scroll_pos();

    render();
}

void View::set_layout_type_singlepage() { view_layout.set_layout_type(PanelLayoutType::SinglePage); }
void View::set_layout_type_continuous() { view_layout.set_layout_type(PanelLayoutType::Continuous); }
void View::set_layout_type_facing() { view_layout.set_layout_type(PanelLayoutType::Facing); }
void View::set_layout_type_continuousfacing() {
    view_layout.set_layout_type(PanelLayoutType::ContinuousFacing);
}

// A vertical Page Slot for Continous Layout modes consists of the following elements so that spacer
// is abover page slot-- in this order:
// -- Page Spacer
// -- top border
// -- Page slot contents
// -- bottom border

// A vertical Page Slot for SinglePage or Facing Layout modes consists of the following elements (no
// vertical spacer) -- in this order:
// -- top border
// -- Page slot contents
// -- bottom border

// A horizontal page slot for facing Layout modes consist of the following elements so that page
// slot is centered -- in this order:
// -- 0.5 * page spacer
// -- left border
// -- Page slot contents
// -- right border
// -- 0.5 * page spacer

// A horizontal page Slot for Single and continuous (e.g. non-facing) Layout modes consists of the
// following elements (no horizontal spacer) -- in this order:
// -- left border
// -- Page slot contents
// -- right border

void View::PopulateLayout() {
    auto layout_type = view_layout.get_layout_type();

    auto pg_cnt = g_document->get_page_count();

    std::vector<Panel> panels;

    if (layout_type == PanelLayoutType::SinglePage || layout_type == PanelLayoutType::Continuous) {
        for (uint32_t i = 0; i < pg_cnt; i++) {
            auto sz = g_document->get_page_dims(i);
            Slot s(Rect(0, 0, sz.width, sz.height), 1.f, i);
            panels.push_back(Panel(s));
        }
    }
    // 2-col Facing Layout
    else {
        for (uint32_t i = 0; i < pg_cnt; i += 2) {
            auto sz = g_document->get_page_dims(i);
            Slot s1(Rect(0, 0, sz.width, sz.height), 1.f, i);

            // do not add the final slot for odd number of pages
            if (i < pg_cnt - 1) {
                auto sz = g_document->get_page_dims(i+1);
                Slot s2(Rect(0, 0, sz.width, sz.height), 1.f, i+1);
                std::vector<Slot> slots = {s1, s2};
                panels.push_back(Panel(slots));
            } else {
                panels.push_back(Panel(s1));
            }
        }
    }

    view_layout.reset_panels(panels);
}

///////////////////////////////////////////////////////////////////////////////////////////
// Zoom Methods

inline void View::invalidate_render_cache_for_zoom() {
  g_document->invalidate_render_cache();
}


void View::zoom_event(void (View::*zoom_func)()) {
    float prev_zoom = zoom.get_zoom_level();
    (this->*zoom_func)();
    float new_zoom = zoom.get_zoom_level();

    printf("new zoom level: %f      (prev level: %f)\n", new_zoom, prev_zoom);

    if (prev_zoom == new_zoom) {
        return;
    }

    // Invalidate Document Render Cache when changing zoom level
    invalidate_render_cache_for_zoom();

    // Calculate revised current_panel_position for new zoom level
    float zoom_factor = new_zoom / prev_zoom;

    uint32_t border_width = view_layout.get_params().get_border_width_px();
    uint32_t padding_width = view_layout.get_params().get_padding_px();
    int slot_page_offset =
            view_layout.get_current_panel_position() + static_cast<int>(padding_width + border_width);

    // if we are in the border/padding area, leave as-is. Only if it remains < 0, then re-calculate
    if (slot_page_offset < 0) {
        slot_page_offset = static_cast<int>(static_cast<float>(slot_page_offset) * zoom_factor);

        // Add the padding and border back into the new offset
        int new_panel_position = slot_page_offset - static_cast<int>(padding_width + border_width);

        view_layout.set_current_panel(
                view_layout.get_current_panel_index(), new_panel_position);
    }

    // Recalculate Scrolling
    calculate_scroll_range();
    calculate_scroll_pos();

    render();
}

void View::zoom_in() { zoom.zoom_in(); }
void View::zoom_out() { zoom.zoom_out(); }
// Not In Use -- How to handle with param???
void View::zoom_to(float zoom_to) { zoom.zoom_to(zoom_to); }
void View::zoom_actual_size() { zoom.zoom_actual_size(); }

void View::zoom_fit_page() { 

    // Fit page -- Always fits horizontally. As wide as possible to fit as least one full page
    // vertically.
    auto layout_type = view_layout.get_layout_type();
    auto padding_px = view_layout.get_params().get_padding_px();
    auto border_px = view_layout.get_params().get_border_width_px();
    auto current_panel_index = (uint32_t)view_layout.get_current_panel_index();

    int pagelayout_width = 0;
    int pagelayout_height = 0;
    if (layout_type == PanelLayoutType::Facing || layout_type == PanelLayoutType::ContinuousFacing) {
        auto page1_dims = g_document->get_page_dims(current_panel_index);

        // setting page2 default to -border_px to offset the space between both pages
        Size page2_dims = {-static_cast<int>(padding_px), 0};
        if (current_panel_index < g_document->get_page_count()) {
            page2_dims = g_document->get_page_dims(current_panel_index + 1);
        }

        pagelayout_width = page1_dims.width + page2_dims.width + (3 * static_cast<int>(padding_px)) + (4 * static_cast<int>(border_px));
        pagelayout_height =
                std::max(page1_dims.height, page2_dims.height) + (2 * static_cast<int>(padding_px)) + (2 * static_cast<int>(border_px));
    } else {  // SinglePage || Continuous
        Size page_dims = g_document->get_page_dims(current_panel_index);

        pagelayout_width = page_dims.width + (2 * static_cast<int>(padding_px)) +
                           (2 * static_cast<int>(border_px));
        pagelayout_height = page_dims.height + (2 * static_cast<int>(padding_px)) +
                            (2 * static_cast<int>(border_px));
    }

    float scale_horizontal = pagelayout_width > 0 ? static_cast<float>(view_viewport.width()) /
                                                            static_cast<float>(pagelayout_width)
                                                  : 1.0f;
    float scale_vertical = pagelayout_height > 0 ? static_cast<float>(view_viewport.height()) /
                                                           static_cast<float>(pagelayout_height)
                                                 : 1.0f;


    zoom.zoom_fit_page(std::min(scale_horizontal, scale_vertical)); 

}
void View::zoom_fit_width() {
    // Fit page -- Always fits horizontally. As wide as possible to fit as least one full page
    // vertically.
    auto layout_type = view_layout.get_layout_type();
    auto padding_px = view_layout.get_params().get_padding_px();
    auto border_px = view_layout.get_params().get_border_width_px();
    auto current_panel_index = (uint32_t)view_layout.get_current_panel_index();

    int pagelayout_width = 0;
    //int pagelayout_height = 0;
    if (layout_type == PanelLayoutType::Facing || layout_type == PanelLayoutType::ContinuousFacing) {
        auto page1_dims = g_document->get_page_dims(current_panel_index);

        // setting page2 default to -border_px to offset the space between both pages
        Size page2_dims = {-static_cast<int>(padding_px), 0};
        if (current_panel_index < g_document->get_page_count()) {
            page2_dims = g_document->get_page_dims(current_panel_index + 1);
        }

        pagelayout_width = page1_dims.width + page2_dims.width +
                           (3 * static_cast<int>(padding_px)) + (4 * static_cast<int>(border_px));
    } else {  // SinglePage || Continuous
        Size page_dims = g_document->get_page_dims(current_panel_index);

        pagelayout_width = page_dims.width + (2 * static_cast<int>(padding_px)) +
                           (2 * static_cast<int>(border_px));
    }

    float scale_horizontal = pagelayout_width > 0 ? static_cast<float>(view_viewport.width()) /
                                                            static_cast<float>(pagelayout_width)
                                                  : 1.0f;

    zoom.zoom_fit_width(scale_horizontal);
}

void View::zoom_fit_visible() { zoom.zoom_fit_visible(); }

///////////////////////////////////////////////////////////////////////////////////////////
// Scrolling

void View::scroll_event(void (View::*scroll_func)()) {
    (this->*scroll_func)();

    view_layout.set_current_panel_from_scroll_pos(scroll.get_scroll_pos(), zoom.get_zoom_level());

    printf("- scroll_pos: %d   cur_panel_index: %d    cur_panel_pos: %d\n",
           scroll.get_scroll_pos(),
           view_layout.get_current_panel_index(),
           view_layout.get_current_panel_position());

    render();
}

void View::scroll_home() {
    auto range = scroll.get_scroll_range();
    scroll.set_scroll_pos(range.min, true);

    printf("scrolling: home ");
}

void View::scroll_end() {
    auto range = scroll.get_scroll_range();
    scroll.set_scroll_pos(range.max, true);
    printf("scrolling: end ");
}

void View::scroll_lineup() {
    scroll.scroll_lineup();
    printf("scrolling: lineup ");
}
void View::scroll_linedown() {
    scroll.scroll_linedown();
    printf("scrolling: linedown ");
}
void View::scroll_pageup() {
    scroll.scroll_pageup();
    printf("scrolling: pageup ");
}
void View::scroll_pagedown() {
    scroll.scroll_pagedown();
    printf("scrolling: pagedown ");
}

void View::calculate_scroll_range() {
    // Calc total height
    int layout_height = view_layout.get_layout_height(zoom.get_zoom_level());

    // subtract viewport height from total height to get scroll range
    int max_range = layout_height - view_viewport.size().height;

    scroll.set_scroll_range(0, max_range);
}

void View::calculate_scroll_pos() {
    int pos = view_layout.calculate_pos_for_current_panel_info(zoom.get_zoom_level());
    
    // ensure pos is within range
    pos = std::max(pos, scroll.get_scroll_range().min);
    pos = std::min(pos, scroll.get_scroll_range().max);
    
    scroll.set_scroll_pos(pos);
}


///////////////////////////////////////////////////////////////////////////////////////////
// Page Resize
void View::resizeEvent(ResizeEvent& e) {
    if (e.width <= 0 || e.height <= 0) {
        return;
    }

    view_viewport = Viewport(0, 0, e.width, e.height);


    // Recalculate Scroll
    calculate_scroll_range();
    calculate_scroll_pos();

    uint32_t scroll_page = e.height < 40 ? (uint32_t)e.height : (uint32_t)e.height - 40;
    scroll.set_scroll_pagesize(scroll_page);


    // Maintain same ZoomMode
    switch (zoom.get_zoom_mode()) {
        case ZoomMode::ActualSize:
            zoom_actual_size();
            break;
        case ZoomMode::FitPage:
            zoom_fit_page();
            break;
        case ZoomMode::FitWidth:
            zoom_fit_width();
            break;
        case ZoomMode::FitVisible:
            zoom_fit_visible();
            break;
        default:
            break;
    }

    render();
}


///////////////////////////////////////////////////////////////////////////////////////////
// Page Render
void View::CalculateVisiblePanels() {
    visible_panels.clear();
    
    auto layout_type = view_layout.get_layout_type();

    Point viewport_center(view_viewport.center_x(), view_viewport.center_y());

    switch (layout_type) {
        case PanelLayoutType::SinglePage:
            CalculateVisiblePanels_SinglePage(viewport_center);
            break;
        case PanelLayoutType::Facing:
            CalculateVisiblePanels_Facing(viewport_center);
            break;
        case PanelLayoutType::Continuous:
            CalculateVisiblePanels_Continuous(viewport_center);
            break;
        case PanelLayoutType::ContinuousFacing:
            CalculateVisiblePanels_ContinuousFacing(viewport_center);
            break;
    }
}

void View::render() {
    CalculateVisiblePanels();
    update();  // trigger PaintEvent
}


void View::CalculateVisiblePanels_SinglePage(Point viewport_center) {
    auto zoom_level = zoom.get_zoom_level();
    
    Size page_size = g_document->get_page_dims(static_cast<uint32_t>(view_layout.get_current_panel_index()));

    page_size.width = static_cast<int>(static_cast<float>(page_size.width) * zoom_level);
    page_size.height = static_cast<int>(static_cast<float>(page_size.height) * zoom_level);

    // Calculate viewport x, y position for this single page.
    int page_x = viewport_center.x - (page_size.width / 2);
    int page_y = viewport_center.y - (page_size.height / 2);

    // set page pos to 0 or negative value -- resets current page to top
    view_layout.set_current_panel(view_layout.get_current_panel_index(), std::min(0, page_y));

    // set page pos to 0 or negative value
    //current_page_position = std::min(0, page_y);
    Slot s(Rect(page_x, page_y, page_size.width, page_size.height), zoom_level, static_cast<uint32_t>(view_layout.get_current_panel_index()));
    visible_panels.push_back(Panel(s));
}

void View::CalculateVisiblePanels_Facing(Point viewport_center) {

    // if we are on the last page, use SinglePage
    if (static_cast<int>(g_document->get_page_count()) == view_layout.get_current_panel_index() - 1) {
        CalculateVisiblePanels_SinglePage(viewport_center);
        return;
    }

    // Create layout for side-by-side pages
    auto zoom_level = zoom.get_zoom_level();
    auto padding_px = view_layout.get_params().get_padding_px();
    Size page1_dims =
            g_document->get_page_dims(static_cast<uint32_t>(view_layout.get_current_panel_index()));

    page1_dims.width = static_cast<int>(static_cast<float>(page1_dims.width) * zoom_level);
    page1_dims.height = static_cast<int>(static_cast<float>(page1_dims.height) * zoom_level);


    Size page2_dims =
            g_document->get_page_dims(static_cast<uint32_t>(view_layout.get_current_panel_index()+1));

    page2_dims.width = static_cast<int>(static_cast<float>(page2_dims.width) * zoom_level);
    page2_dims.height = static_cast<int>(static_cast<float>(page2_dims.height) * zoom_level);


    // Calculate offset from center -- if facing page widths are different
    int center_offset = (page1_dims.width - page2_dims.width) / 2;

    // Calculate viewport x, y position for this single page.
    int page1_x = viewport_center.x + center_offset - page1_dims.width - static_cast<int>(padding_px) / 2;
    int page2_x = viewport_center.x + center_offset + static_cast<int>(padding_px) / 2;

    int page1_y = viewport_center.y - (page1_dims.height / 2);
    int page2_y = viewport_center.y - (page2_dims.height / 2);

    // set page pos to 0 or negative value
    view_layout.set_current_panel(view_layout.get_current_panel_index(), std::min(0, page1_y));

    // Create Page1
    Slot s1(Rect(page1_x, page1_y, page1_dims.width, page1_dims.height), zoom_level, view_layout.get_current_panel_index());
    visible_panels.push_back(Panel(s1));

    // Create Page1
    Slot s2(Rect(page2_x, page2_y, page2_dims.width, page2_dims.height), zoom_level, view_layout.get_current_panel_index()+1);
    visible_panels.push_back(Panel(s2));
}


void View::CalculateVisiblePanels_Continuous(Point viewport_center) {
    auto zoom_level = zoom.get_zoom_level();

    uint32_t border_width = view_layout.get_params().get_border_width_px();
    uint32_t padding_width = view_layout.get_params().get_padding_px();

    // Ensure that the page pos is not greater than the spacing so that we start at the top of the
    // viewport (e.g. switching from SinglePage layout)
    auto temp_pos = view_layout.get_current_panel_position() + static_cast<int>(border_width + padding_width);

    //std::min(view_layout.get_current_panel_position(),
            //         static_cast<int>(view_layout.get_vertical_slot_offset()));

    // Loop through documents until (1) no more pages remain or (2) viewport is filled to bottom of
    // screen
    int temp_panel_index = view_layout.get_current_panel_index();
    int viewport_height = view_viewport.height();
    uint32_t pg_count = g_document->get_page_count();


    assert(temp_pos <= viewport_height);
    while (temp_pos <= viewport_height && (uint32_t)temp_panel_index < pg_count) {

        int page_index = temp_panel_index++;
        Size page_dims = g_document->get_page_dims(static_cast<uint32_t>(page_index));
        page_dims.width = static_cast<int>(static_cast<float>(page_dims.width) * zoom_level);
        page_dims.height = static_cast<int>(static_cast<float>(page_dims.height) * zoom_level);


        // Calculate viewport x, y position for this single page.
        int page_x = viewport_center.x - (page_dims.width / 2);
        int page_y = temp_pos;

        // Add Page Slot to visible panels vector
        Slot s(Rect(page_x, page_y, page_dims.width, page_dims.height), zoom_level, page_index);
        Panel p(s);
        int panel_height = p.get_panel_rect().height;
        visible_panels.push_back(Panel(s));

        uint32_t slot_height = padding_width + border_width + (uint32_t)panel_height + border_width;

        temp_pos += slot_height;
    }

    // Now, if we have not reached the bottom and have slotted **all** pages, center them.
}

void View::CalculateVisiblePanels_ContinuousFacing(Point viewport_center) {

    auto zoom_level = zoom.get_zoom_level();
    uint32_t border_width = view_layout.get_params().get_border_width_px();
    uint32_t padding_width = view_layout.get_params().get_padding_px();

    // Ensure that the page pos is not greater than the spacing so that we start at the top of the
    // viewport (e.g. switching from SinglePage layout)
    //current_page_position =
    //        std::max(current_page_position, static_cast<int>(viewer_params.get_page_spacer_px()));
    auto temp_pos = view_layout.get_current_panel_position() +
                    static_cast<int>(border_width + padding_width);


    // Loop through documents until (1) no more pages remain or (2) viewport is filled to bottom of
    // screen
    uint32_t temp_panel_index = (uint32_t)view_layout.get_current_panel_index();
    assert(temp_pos <= view_viewport.height());
    while (temp_pos <= view_viewport.height() && temp_panel_index < g_document->get_page_count()) {
        // Create layout for side-by-side pages
        uint32_t page1_index = temp_panel_index++;
        Size page1_dims = g_document->get_page_dims(static_cast<uint32_t>(page1_index));
        page1_dims.width = static_cast<int>(static_cast<float>(page1_dims.width) * zoom_level);
        page1_dims.height = static_cast<int>(static_cast<float>(page1_dims.height) * zoom_level);


        Size page2_dims(0, 0);
        uint32_t page2_index = 0;
        if (temp_panel_index < g_document->get_page_count()) {
            page2_index = temp_panel_index++;
            page2_dims = g_document->get_page_dims(static_cast<uint32_t>(page2_index));
            page2_dims.width = static_cast<int>(static_cast<float>(page2_dims.width) * zoom_level);
            page2_dims.height = static_cast<int>(static_cast<float>(page2_dims.height) * zoom_level);
        }

        // Calculate offset from center -- if facing page widths are different
        int center_offset = (page1_dims.width - page2_dims.width) / 2;

        // Calculate viewport x, y position for this single page.
        int page1_x = viewport_center.x + center_offset - page1_dims.width -
                      static_cast<int>(padding_width) / 2;
        int page2_x = viewport_center.x + center_offset + static_cast<int>(padding_width) / 2;

        // PdfPage slot Y is centered with each other -- even for different page sizes
        int facing_slot_height = std::max(page1_dims.height, page2_dims.height);
        int page1_y = temp_pos + ((facing_slot_height - page1_dims.height) / 2);
        int page2_y = temp_pos + ((facing_slot_height - page2_dims.height) / 2);

        // Create Page1
        Slot s1(Rect(page1_x, page1_y, page1_dims.width, page1_dims.height), zoom_level, page1_index);
        visible_panels.push_back(Panel(s1));

        // Create Page2
        if (page2_index > page1_index) { //page2_dims.width > 0) {
            Slot s2(Rect(page2_x, page2_y, page2_dims.width, page2_dims.height), zoom_level, page2_index);
            visible_panels.push_back(Panel(s2));
        }

        uint32_t slot_height =
                padding_width + border_width + (uint32_t)facing_slot_height + border_width;
        temp_pos += slot_height;
    }

    // Now, if we have not reached the bottom and have slotted **all** pages, center them.


}


// Paint
void View::paintEvent(PaintEvent& /*event*/) {
    Painter p(this);

    int l = 0;
    int t = 0;
    int w = width();
    int h = height();

    p.setColor(Lightgrey);
    p.drawRect(l, t, w, h);

    //p.setColor(Green);

    //auto range = scroll.get_scroll_range();
    //auto test_t = range.max - scroll.get_scroll_pos();

    //p.drawRect(l, t, 25, test_t);

    //p.setColor(Red);
    //p.drawRect(26, t, 350, 732 - 2 - scroll.get_scroll_pos());  // 610-130-10);
    //printf("red = %d\n", 732 - 2 - scroll.get_scroll_pos());

    const auto& params = view_layout.get_params();

    typedef std::chrono::high_resolution_clock clock;
     auto t0 = clock::now();

    for (auto panel : visible_panels) {
        panel.paint_event(p, params.get_border_width_px(), params.get_border_color());
    }

    auto t1 = clock::now();
    std::chrono::duration<double, std::milli> fp_ms = t1 - t0;
    std::cout << "Rendering time: " << fp_ms.count() << " ms\n";


    auto range = scroll.get_scroll_range();
    int layout_height = view_layout.get_layout_height(zoom.get_zoom_level());
    printf("layout_height: %d   viewport.height: %d    scroll_range: %d-%d - scroll_pos: %d   cur_panel_index: %d    cur_panel_pos: %d\n",
           layout_height, view_viewport.size().height,
        range.min, range.max, scroll.get_scroll_pos(),
           view_layout.get_current_panel_index(),
           view_layout.get_current_panel_position());
}

bool View::keyPressEvent(KeyEvent& e) {
    std::cout << __func__ << " key: " << e.key() << " modifiers: " << e.modifiers() << std::endl;

    bool result = true;
    switch (e.modifiers()) {
        case KeyEvent::MLCtrl:
        case KeyEvent::MRCtrl:
            switch (e.key()) {
                case KeyEvent::KKeypadPlus:
                case KeyEvent::KPlus:
                case KeyEvent::KEquals:
                    zoom_event(&View::zoom_in);
                    break;
                case KeyEvent::KKeypadMinus:
                case KeyEvent::KMinus:
                    zoom_event(&View::zoom_out);
                    break;
                case KeyEvent::K1:
                    zoom_event(&View::zoom_actual_size);
                    break;
                case KeyEvent::K2:
                    zoom_event(&View::zoom_fit_page);
                    break;
                case KeyEvent::K3:
                    zoom_event(&View::zoom_fit_width);
                    break;
                case KeyEvent::K4:
                    zoom_event(&View::zoom_fit_visible);
                    break;
                case KeyEvent::K7:
                    layout_event(&View::set_layout_type_singlepage);
                    //render();
                    break;
                case KeyEvent::K8:
                    layout_event(&View::set_layout_type_continuous);
                    //render();
                    break;
                case KeyEvent::K9:
                    layout_event(&View::set_layout_type_facing);
                    //render();
                    break;
                case KeyEvent::K0:
                    layout_event(&View::set_layout_type_continuousfacing);
                    //render();
                    break;
                case KeyEvent::KUp:
                    scroll_event(&View::scroll_home);
                    break;
                case KeyEvent::KDown:
                    scroll_event(&View::scroll_end);
                    break;
                case KeyEvent::KHome:
                    scroll_event(&View::scroll_home);
                    //render();
                    break;
                case KeyEvent::KEnd:
                    scroll_event(&View::scroll_end);
                    // setCursor((int)((*textBuffer_)[textBuffer_->size() - 1].size()),
                    //          (int)(textBuffer_->size() - 1));
                    //render();
                    break;
                case KeyEvent::KN: {
                    render();
                    break;
                }
                case KeyEvent::KZ: {
                    // Coord c = cursor();
                    // textBuffer_->undo(c);
                    // setCursor(c);
                    render();
                    break;
                }
                case KeyEvent::KR: {
                    // Coord c = cursor();
                    // textBuffer_->redo(c);
                    // setCursor(c);
                    render();
                    break;
                }
                default:
                    result = false;
                    break;
            }
            break;
        default:
        //case KeyEvent::MNone:
          switch (e.key()) {
            case KeyEvent::KLeft:
              break;
            case KeyEvent::KRight:
              break;
            case KeyEvent::KHome:
              scroll_event(&View::scroll_home);
              break;
            case KeyEvent::KEnd:
              scroll_event(&View::scroll_end);
              break;
            case KeyEvent::KUp:
              scroll_event(&View::scroll_lineup);
              break;
            case KeyEvent::KDown:
              scroll_event(&View::scroll_linedown);
              break;
            case KeyEvent::KPageUp:
              scroll_event(&View::scroll_pageup);
              break;
            case KeyEvent::KPageDown:
              scroll_event(&View::scroll_pagedown);
              break;

            case KeyEvent::KEscape:
              // escStatusBar();
              break;
            case KeyEvent::KDelete:
              // escStatusBar();
              // if (deleteSelected() == 0)
              //{
              //  //textBuffer_->del(cursor_);
              //  //setCursor(cursor_);
              //  render();
              //}
              break;
            case KeyEvent::KBackspace:
              // if (deleteSelected() == 0)
              //{
              //  //textBuffer_->backspace(cursor_);
              //  //setCursor(cursor_);
              //  render();
              //}
              break;
            case KeyEvent::KReturn: {
              // deleteSelected();
              // escStatusBar();
              // textBuffer_->insert(cursor_, L"\n");
              // setCursor(cursor_);
              render();
              break;
            }
            default:
              result = false;
              break;
          };
    }

    return result; 
}


}  // namespace PdfViewer::Viewer