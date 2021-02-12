#include "viewer/zoom.h"
#include <algorithm>


namespace PdfViewer::Viewer {

Zoom::Zoom(float zoom_level) {
    zoom_info = {ZoomMode::Percentage, zoom_level};
    fit_page_value = 1.0f;
    fit_width_value = 1.0f;
    fit_visible_value = 1.0f;

    defined_zoom_levels.insert(
            defined_zoom_levels.end(),
            {.01f, .0833f, .125f, .25f, .3333f, .5f,  .6667f, .75f,  1.f,   1.25f,
             1.5f, 2.0f,   3.0f,  4.0f, 6.0f,   8.0f, 12.0f,  16.0f, 32.0f, 64.0f});
}

Zoom::~Zoom() {
    
}

float Zoom::get_zoom_level() const { return zoom_info.zoom_level; }
ZoomMode Zoom::get_zoom_mode() const { return zoom_info.mode; }

float Zoom::zoom_in() {
    zoom_info.zoom_level = get_next_defined_zoom_in_level();
    zoom_info.mode = ZoomMode::Percentage;
    return zoom_info.zoom_level;
}

float Zoom::zoom_out() {
    zoom_info.zoom_level = get_next_defined_zoom_out_level();
    zoom_info.mode = ZoomMode::Percentage;
    return zoom_info.zoom_level;
}

float Zoom::zoom_to(float zoom_level) {
    zoom_info.zoom_level = zoom_level;
    zoom_info.mode = ZoomMode::Percentage;
    return zoom_info.zoom_level;
}

float Zoom::zoom_actual_size() {
    zoom_info.zoom_level = 1.0f;
    zoom_info.mode = ZoomMode::ActualSize;
    return zoom_info.zoom_level;
}

float Zoom::zoom_fit_page(float zoom_value) {
    fit_page_value = zoom_value;
    zoom_info.zoom_level = fit_page_value;
    zoom_info.mode = ZoomMode::FitPage;
    return zoom_info.zoom_level;
}

float Zoom::zoom_fit_width(float zoom_value) {
    fit_width_value = zoom_value;
    zoom_info.zoom_level = fit_width_value;
    zoom_info.mode = ZoomMode::FitWidth;
    return zoom_info.zoom_level;
}

float Zoom::zoom_fit_visible() {
    zoom_info.zoom_level = fit_visible_value;
    zoom_info.mode = ZoomMode::FitVisible;
    return zoom_info.zoom_level;
}

void Zoom::set_zoom_mode_values(float fit_page_value,
                                float fit_width_value,
                                float fit_visible_value) {
    this->fit_page_value = fit_page_value;
    this->fit_width_value = fit_width_value;
    this->fit_visible_value = fit_visible_value;
}

void Zoom::set_defined_zoom_levels(std::vector<float> defined_zoom_levels) {
    this->defined_zoom_levels = defined_zoom_levels;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Private methods

float Zoom::get_next_defined_zoom_in_level() {
    float temp_zoom_level = zoom_info.zoom_level;

    // Find next larger zoom level
    auto upper = std::upper_bound(
            defined_zoom_levels.begin(), defined_zoom_levels.end(), temp_zoom_level);

    if (upper != defined_zoom_levels.end()) {
        temp_zoom_level = *upper;
    }

    return temp_zoom_level;
}

float Zoom::get_next_defined_zoom_out_level() {
    float temp_zoom_level = zoom_info.zoom_level;

    // Finds the current zoom_level in vector
    auto lower = std::lower_bound(
            defined_zoom_levels.begin(), defined_zoom_levels.end(), temp_zoom_level);

    // If we found and is not the first entry, point to previous entry
    if (lower != defined_zoom_levels.end() && lower != defined_zoom_levels.begin()) {
        temp_zoom_level = *(lower - 1);
    }

    return temp_zoom_level;
}

}  // namespace PdfViewer::Viewer