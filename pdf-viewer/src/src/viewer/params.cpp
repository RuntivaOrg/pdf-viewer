#include "viewer/params.h"
#include <algorithm>


namespace PdfViewer::Viewer {

Params::Params(Color background_color,
               uint32_t padding_px,
               uint32_t border_width_px,
               Color border_color)
        : background_color(background_color)
        , padding_px(padding_px)
        , border_width_px(border_width_px)
        , border_color(border_color) {}

///////////////////////////////////////////////////////////////////////////////////////////
// Const params

Color Params::get_background_color() const { return background_color; }
uint32_t Params::get_padding_px() const { return padding_px; }
uint32_t Params::get_border_width_px() const { return border_width_px; }
Color Params::get_border_color() const { return border_color; }

}  // namespace PdfViewer::Viewer