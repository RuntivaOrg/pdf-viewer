#ifndef PDFVIEWER_VIEWER_VIEW_H_DEFINED
#define PDFVIEWER_VIEWER_VIEW_H_DEFINED
#pragma once

#include "../widget.h"

#include "geometry.h"
#include "panel_layout.h"
#include "document.h"
#include "zoom.h"
#include "scroll.h"
//#include "params.h"
#include <vector>

namespace PdfViewer::Viewer {

/// <summary>
/// A view is attached to a document and acts as an intermediary between the document and the user:
/// the view renders an image of the document on the screen and interprets user input as operations
/// upon the document.
/// </summary>
class View : public Widget {
public:
    View(Widget* parent, Params params, PanelLayoutType type);
    ~View() final;

protected:
    void resizeEvent(ResizeEvent&) override;
    void paintEvent(PaintEvent&) override;
    bool keyPressEvent(KeyEvent&) override;

private:
    PanelLayout view_layout;
    Viewport view_viewport;

    Zoom zoom;
    Scroll scroll;

    // Current Visible Panels
    std::vector<Panel> visible_panels;

    // Layout Type Methods
    void layout_event(void (View::*layout_func)());
    void set_layout_type_singlepage();
    void set_layout_type_continuous();
    void set_layout_type_facing();
    void set_layout_type_continuousfacing();

    void PopulateLayout();

    // Zoom Methods
    void zoom_event(void (View::*zoom_func)());
    void invalidate_render_cache_for_zoom();
    void zoom_in();
    void zoom_out();
    void zoom_to(float zoom_to);
    void zoom_actual_size();
    void zoom_fit_page();
    void zoom_fit_width();
    void zoom_fit_visible();


    // Scroll
    void scroll_event(void (View::*scroll_func)());
    void scroll_home();
    void scroll_end();
    void scroll_lineup();
    void scroll_linedown();
    void scroll_pageup();
    void scroll_pagedown();

    void calculate_scroll_range();
    void calculate_scroll_pos();

    // Render
    void CalculateVisiblePanels();
    void CalculateVisiblePanels_SinglePage(Point viewport_center);
    void CalculateVisiblePanels_Facing(Point viewport_center);
    void CalculateVisiblePanels_Continuous(Point viewport_center);
    void CalculateVisiblePanels_ContinuousFacing(Point viewport_center);

    void render(); 
};

}  // namespace PdfViewer::Viewer
#endif  // PDFVIEWER_VIEWER_VIEW_H_DEFINED
