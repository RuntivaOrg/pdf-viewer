#ifndef PDFVIEWER_VIEWER_PARAMS_H_DEFINED
#define PDFVIEWER_VIEWER_PARAMS_H_DEFINED
#pragma once

#include "../color.h"

namespace PdfViewer::Viewer {

//////////////////////////////////////////////////////////////
/// Viewer Params class manages configuration styling settings
class Params {
public:
    Params(Color background_color = Lightgrey,
                 uint32_t padding_px = 10,
                 uint32_t border_width_px = 1,
                 Color border_color = Black);

    // Const Params
    Color get_background_color() const;
    uint32_t get_padding_px() const;
    uint32_t get_border_width_px() const;
    Color get_border_color() const;

private:
    const Color background_color;
    const uint32_t padding_px;
    const uint32_t border_width_px;
    const Color border_color;
};
}  // namespace PdfViewer::Viewer
#endif  // PDFVIEWER_VIEWER_PARAMS_H_DEFINED
