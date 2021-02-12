#ifndef PDFVIEWER_VIEWER_DOCUMENT_H_DEFINED
#define PDFVIEWER_VIEWER_DOCUMENT_H_DEFINED
#pragma once

#include "pdfium/free_deleter.h"
#include "pdfium/file_access_test_loader.h"

#include "core/rendered_page_cache.h"

#include <stdio.h>
#include <stdlib.h>

#include "geometry.h"
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <map>

#if defined(PDF_ENABLE_SKIA) && !defined(_SKIA_SUPPORT_)
#define _SKIA_SUPPORT_
#endif

#if defined(PDF_ENABLE_SKIA_PATHS) && !defined(_SKIA_SUPPORT_PATHS_)
#define _SKIA_SUPPORT_PATHS_
#endif

// Undefining __WINDOWS__ so that fpdfview.h can properly build on _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreserved-id-macro"
#ifdef __WINDOWS__
#undef __WINDOWS__
#endif
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonportable-system-include-path"
//#pragma GCC diagnostic ignored "-Wunused-member-function"
#include "public/cpp/fpdf_scopers.h"
#include "public/fpdf_annot.h"
#include "public/fpdf_attachment.h"
#include "public/fpdf_dataavail.h"
#include "public/fpdf_edit.h"
#include "public/fpdf_ext.h"
#include "public/fpdf_formfill.h"
#include "public/fpdf_progressive.h"
#include "public/fpdf_structtree.h"
#include "public/fpdf_edit.h"
#include "public/fpdf_text.h"
#include "public/fpdfview.h"
#pragma GCC diagnostic pop


#ifdef _WIN32
#define access _access
#define snprintf _snprintf
#define R_OK 4
#endif



enum class OutputFormat {
  kNone,
  kPageInfo,
  kStructure,
  kText,
  kPpm,
  kBgra32,
  kBgr24,
  kRgb24,
  kPng,
  kAnnot,
#ifdef _WIN32
  kBmp,
  kEmf,
  kPs2,
  kPs3,
#endif
#ifdef PDF_ENABLE_SKIA
  kSkp,
#endif
};


  //bool no_smoothtext = false;
  //bool no_smoothimage = false;
  //bool no_smoothpath = false;
  //bool reverse_byte_order = false;
  //bool save_attachments = false;
  //bool save_images = false;
  //bool save_rendered_images = false;
  //bool save_thumbnails = false;
  //bool save_thumbnails_decoded = false;
  //bool save_thumbnails_raw = false;
// #ifdef PDF_ENABLE_V8
//   bool disable_javascript = false;
//   std::string js_flags;  // Extra flags to pass to v8 init.
// #ifdef PDF_ENABLE_XFA
//   bool disable_xfa = false;
// #endif  // PDF_ENABLE_XFA
// #endif  // PDF_ENABLE_V8
//   //bool pages = false;
//   //bool md5 = false;
// #ifdef ENABLE_CALLGRIND
//   bool callgrind_delimiters = false;
// #endif
// #if defined(__APPLE__) || (defined(__linux__) && !defined(__ANDROID__))
//   bool linux_no_system_fonts = false;
// #endif


struct Options {
  [[maybe_unused]]
  Options();
  ~Options();

  bool show_config = false;
  bool show_metadata = false;
  //bool send_events = false;
  //bool use_load_mem_document = false;
  //bool render_oneshot = false;
  bool lcd_text = false;
  bool no_nativetext = false;
  bool grayscale = false;
  //bool forced_color = false;
  bool fill_to_stroke = false;
  bool limit_cache = false;
  bool force_halftone = false;
  bool printing = false;


  OutputFormat output_format = OutputFormat::kNone;
  std::string password;
  std::string scale_factor_as_string;
  //std::string exe_path;
  //std::string bin_directory;
  //std::string font_directory;
  int first_page = 0;  // First 0-based page number to renderer.
  int last_page = 0;   // Last 0-based page number to renderer.
  time_t time = -1;
};


struct FPDF_FORMFILLINFO_PDFiumTest final : public FPDF_FORMFILLINFO {
  FPDF_FORMFILLINFO_PDFiumTest();
  //FPDF_FORMFILLINFO_PDFiumTest(std::map<int, ScopedFPDFPage> loaded_pages, FPDF_FORMHANDLE form_handle);
  ~FPDF_FORMFILLINFO_PDFiumTest();
  FPDF_FORMFILLINFO_PDFiumTest(const FPDF_FORMFILLINFO_PDFiumTest&) = delete;
  
  FPDF_FORMFILLINFO_PDFiumTest& operator=(
      const FPDF_FORMFILLINFO_PDFiumTest& other) {
    
      // creating a new map -- only time we copy is initialization
      loaded_pages = std::map<int, ScopedFPDFPage>();

      return *this;
  }

  // Hold a map of the currently loaded pages in order to avoid them
  // to get loaded twice.
  std::map<int, ScopedFPDFPage> loaded_pages;
  //std::unique_ptr<std::map<int, ScopedFPDFPage>> loaded_pages;

  // Hold a pointer of FPDF_FORMHANDLE so that PDFium app hooks can
  // make use of it.
  FPDF_FORMHANDLE form_handle;
};

int PageRenderFlagsFromOptions(const Options& options);
FPDF_FORMFILLINFO_PDFiumTest* ToPDFiumTestFormFillInfo(
    FPDF_FORMFILLINFO* form_fill_info);





namespace PdfViewer::Viewer {


/// <summary>
/// A view is attached to a document and acts as an intermediary between the document and the user:
/// the view renders an image of the document on the screen and interprets user input as operations
/// upon the document.
/// </summary>
class Document {
public:
    Document(std::string name);
    ~Document();

[[maybe_unused]]
    void set_options(Options options);
    void load_document();
    std::vector<uint8_t> load_page_ppm(int page_index, float scale, int w, int h);
    void unload_document();

    uint32_t get_page_count() const;
    bool get_page_bbox(const int page_index, float* left,
        float* bottom,
        float* right,
        float* top) const;

    Size get_page_dims(uint32_t page_index) const;

    void invalidate_render_cache();

private:
    Options _options;
    std::string _name;
    uint32_t _page_count;
    
    // Page sizes: Initially populated with first page when available on linearized PDFs, then subseqently populated for all pages.
    class PageSizeData {
     public:
      PageSizeData(int start_pg_index, int end_pg_index, Size pg_size)
          : start_page_index(start_pg_index), end_page_index(end_pg_index), page_size(pg_size) {}
      int start_page_index;
      int end_page_index;
      Size page_size;
    };

    // Page Size Data
    bool page_sizes_populated;
    std::vector<PageSizeData> page_size_data;

    // PDFLibrary Init
    UNSUPPORT_INFO unsupported_info;

    std::unique_ptr<char, pdfium::FreeDeleter> file_contents;
    pdfium::TestLoader* m_pLoader;
    FPDF_FILEACCESS file_access;
    FX_FILEAVAIL file_avail;
    FX_DOWNLOADHINTS hints;

    ScopedFPDFFormHandle form;

    // |pdf_avail| must outlive |doc|.
    ScopedFPDFAvail pdf_avail;

    // |doc| must outlive |form_callbacks.loaded_pages|.
    ScopedFPDFDocument doc;

    FPDF_FORMFILLINFO_PDFiumTest form_callbacks;
    bool _is_linearized;

    RenderedPageCache _rendered_cache;

    void populate_page_size_data(FPDF_DOCUMENT doc,
                                 FPDF_FORMFILLINFO_PDFiumTest* form_fill_info,
                                 uint32_t page_count,
                                 const std::function<void()>& idler);


    void initialize_PDFLibrary();

    void ProcessPdf(const std::string &name,
                const char *buf,
                size_t len,
                const Options &options,
                const std::function<void()> &idler);

    bool ProcessPage(const std::string& name,
                     FPDF_DOCUMENT doc,
                     FPDF_FORMHANDLE form,
                     FPDF_FORMFILLINFO_PDFiumTest* form_fill_info,
                     const int page_index,
                     const Options& options,
                     int width,
                     int height,
                     std::vector<uint8_t>& img_buffer,
                     const std::function<void()>& idler);


    static void print_last_error();
    static FPDF_BOOL is_data_avail(FX_FILEAVAIL* avail, size_t offset, size_t size);
    static void idler();
    static void add_segment(FX_DOWNLOADHINTS* hints, size_t offset, size_t size);
    static FPDF_BOOL need_to_pause_now(IFSDK_PAUSE* p);

    static FPDF_PAGE get_page_for_index(FPDF_FORMFILLINFO* param,
                          FPDF_DOCUMENT doc,
                          int index);
};

}  // namespace PdfViewer::Viewer
#endif  // PDFVIEWER_VIEWER_DOCUMENT_H_DEFINED
