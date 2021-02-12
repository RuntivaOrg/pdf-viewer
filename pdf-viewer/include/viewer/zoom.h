#ifndef PDFVIEWER_VIEWER_ZOOM_H_DEFINED
#define PDFVIEWER_VIEWER_ZOOM_H_DEFINED
#pragma once

#include <vector>

namespace PdfViewer::Viewer {

enum class ZoomMode {
    ActualSize,
    FitPage,
    FitWidth,
    FitVisible,
    Percentage
};

struct ZoomInfo {
    ZoomMode mode;
    float zoom_level;
};

class Zoom {
public:
    Zoom(float zoom_level = 1.0f);
    ~Zoom();
    
    float get_zoom_level() const;
    ZoomMode get_zoom_mode() const;

    /// Zooms in to the next larger defined zoom level. Returns the new zoom level.
    float zoom_in();

    /// Zooms out to the next smaller defined zoom level. Returns the new zoom level.
    float zoom_out();

    /// Zooms to to provided zoom level. Returns the new zoom level.
    float zoom_to(float zoom_level);

    float zoom_actual_size();
    float zoom_fit_page(float zoom_value);
    float zoom_fit_width(float zoom_value);
    float zoom_fit_visible();

    void set_zoom_mode_values(float fit_page_value, float fit_width_value, float fit_visible_value);
    void set_defined_zoom_levels(std::vector<float> defined_zoom_levels);

private:
    ZoomInfo zoom_info;
    float fit_page_value;
    float fit_width_value;
    float fit_visible_value;

    std::vector<float> defined_zoom_levels;

    float get_next_defined_zoom_in_level();
    float get_next_defined_zoom_out_level();
};

}  // namespace PdfViewer::Viewer
#endif  // PDFVIEWER_VIEWER_ZOOM_H_DEFINED
