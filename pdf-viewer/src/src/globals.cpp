#include "globals.h"

// Instantiate Global Variables for App: Options and currently loaded Document

Options* g_options = nullptr;

PdfViewer::Viewer::Document* g_document = nullptr;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wexit-time-destructors"
#pragma GCC diagnostic ignored "-Wglobal-constructors"
//std::vector<uint8_t> g_pg1_img_buf;
//std::vector<uint8_t> g_pg2_img_buf;
#pragma GCC diagnostic pop
