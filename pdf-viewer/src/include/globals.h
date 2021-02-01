#ifndef VIEWER_GLOBALS_H_
#define VIEWER_GLOBALS_H_
#pragma once

#include "viewer/document.h"
#include <vector>

// Global Variables for App: Options and currently loaded Document
extern Options * g_options;
extern PdfViewer::Viewer::Document * g_document;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
extern std::vector<uint8_t> g_pg1_img_buf;
extern std::vector<uint8_t> g_pg2_img_buf;
#pragma GCC diagnostic pop
#endif // VIEWER_GLOBALS_H_
