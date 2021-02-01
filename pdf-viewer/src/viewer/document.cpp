#include "viewer/document.h"
#include "globals.h"

#include "pdfium/file_access_for_testing.h"
#include "pdfium/pdf_test_write_helper.h"

#include <cassert>

#include <functional>
#include <memory>
//#include <sstream>
#include <string>
#include <vector>

[[maybe_unused]] int PageRenderFlagsFromOptions(const Options& options) {
  int flags = FPDF_ANNOT;
  if (options.lcd_text)
    flags |= FPDF_LCD_TEXT;
  if (options.no_nativetext)
    flags |= FPDF_NO_NATIVETEXT;
  if (options.grayscale)
    flags |= FPDF_GRAYSCALE;
  if (options.fill_to_stroke)
    flags |= FPDF_CONVERT_FILL_TO_STROKE;
  if (options.limit_cache)
    flags |= FPDF_RENDER_LIMITEDIMAGECACHE;
  if (options.force_halftone)
    flags |= FPDF_RENDER_FORCEHALFTONE;
  if (options.printing)
    flags |= FPDF_PRINTING;
//   if (options.no_smoothtext)
//     flags |= FPDF_RENDER_NO_SMOOTHTEXT;
//   if (options.no_smoothimage)
//     flags |= FPDF_RENDER_NO_SMOOTHIMAGE;
//   if (options.no_smoothpath)
//     flags |= FPDF_RENDER_NO_SMOOTHPATH;
//   if (options.reverse_byte_order)
//     flags |= FPDF_REVERSE_BYTE_ORDER;
  return flags;
}

FPDF_FORMFILLINFO_PDFiumTest* ToPDFiumTestFormFillInfo(
    FPDF_FORMFILLINFO* form_fill_info) {
  return static_cast<FPDF_FORMFILLINFO_PDFiumTest*>(form_fill_info);
}

namespace {

void ExampleUnsupportedHandler(UNSUPPORT_INFO*, int type) {
  std::string feature = "Unknown";
  switch (type) {
    case FPDF_UNSP_DOC_XFAFORM:
      feature = "XFA";
      break;
    case FPDF_UNSP_DOC_PORTABLECOLLECTION:
      feature = "Portfolios_Packages";
      break;
    case FPDF_UNSP_DOC_ATTACHMENT:
    case FPDF_UNSP_ANNOT_ATTACHMENT:
      feature = "Attachment";
      break;
    case FPDF_UNSP_DOC_SECURITY:
      feature = "Rights_Management";
      break;
    case FPDF_UNSP_DOC_SHAREDREVIEW:
      feature = "Shared_Review";
      break;
    case FPDF_UNSP_DOC_SHAREDFORM_ACROBAT:
    case FPDF_UNSP_DOC_SHAREDFORM_FILESYSTEM:
    case FPDF_UNSP_DOC_SHAREDFORM_EMAIL:
      feature = "Shared_Form";
      break;
    case FPDF_UNSP_ANNOT_3DANNOT:
      feature = "3D";
      break;
    case FPDF_UNSP_ANNOT_MOVIE:
      feature = "Movie";
      break;
    case FPDF_UNSP_ANNOT_SOUND:
      feature = "Sound";
      break;
    case FPDF_UNSP_ANNOT_SCREEN_MEDIA:
    case FPDF_UNSP_ANNOT_SCREEN_RICHMEDIA:
      feature = "Screen";
      break;
    case FPDF_UNSP_ANNOT_SIG:
      feature = "Digital_Signature";
      break;
  }
  printf("Unsupported feature: %s.\n", feature.c_str());
}

}

namespace PdfViewer::Viewer {

Document::Document(std::string name) : _name(name) {



    m_pLoader = nullptr;
    _page_count = 0;
    file_access = {};
    file_avail = {};
    hints = {};
    _options.output_format = OutputFormat::kPng; // kBgra32; //  kRgb24; // kBgra32; // kBgr24;
    form_callbacks = {};

    initialize_PDFLibrary();

}

//Document::~Document() {
//    FPDF_DestroyLibrary();
//}

void Document::set_options(Options options) {
    _options = options;
}

void Document::initialize_PDFLibrary() {
    FPDF_LIBRARY_CONFIG config;
    config.version = 3;
    config.m_pUserFontPaths = nullptr;
    config.m_pIsolate = nullptr;
    config.m_v8EmbedderSlot = 0;
    config.m_pPlatform = nullptr;

    std::function<void()> idler = []() {};

    // const char* path_array[2] = {nullptr, nullptr};
    // Optional<const char*> custom_font_path = GetCustomFontPath(options);
    // if (custom_font_path.has_value()) {
    // path_array[0] = custom_font_path.value();
    // config.m_pUserFontPaths = path_array;
    // }

    FPDF_InitLibraryWithConfig(&config);


    UNSUPPORT_INFO unsupported_info = {};
    unsupported_info.version = 1;
    unsupported_info.FSDK_UnSupport_Handler = ExampleUnsupportedHandler;

    FSDK_SetUnSpObjProcessHandler(&unsupported_info);

    if (_options.time > -1) {
        // This must be a static var to avoid explicit capture, so the lambda can be
        // converted to a function ptr.
        static time_t time_ret = _options.time;
        FSDK_SetTimeFunction([]() { return time_ret; });
        FSDK_SetLocaltimeFunction([](const time_t* tp) { return gmtime(tp); });
    }

}

void Document::load_document() {



    size_t file_length = 0;
#ifdef WASM_BUILD
    std::string filename = "airplane.pdf";
#else
    //std::string filename = "C:\\work\\PDF\\SampleFiles\\airplane.pdf";
    std::string filename = "C:\\work\\PDF\\SampleFiles\\13_afh_ch11.pdf";
#endif

    //std::string filename = "C:\\work\\PDF\\SampleFiles\\receipt.pdf";
    //std::string filename = "C:\\work\\PDF\\SampleFiles\\ipsos.pdf";
    //std::unique_ptr<char, pdfium::FreeDeleter> file_contents =
    file_contents = GetFileContents(filename.c_str(), &file_length);
    if (!file_contents) {
      printf("ERROR: error in file_contents");
      return;
    }
    
    printf("Processing PDF file %s.\n", filename.c_str());


    ProcessPdf(filename, file_contents.get(), file_length, _options, idler);
    idler();

}

void Document::unload_document() {
}


uint32_t Document::get_page_count() const { return _page_count; }

Size Document::get_page_dims(uint32_t page_index) const {
    assert(page_index < _page_count);

    float left, bottom, right, top;

    if (!get_page_bbox(static_cast<int>(page_index), &left, &bottom, &right, &top)) {
        printf("ERROR: unable to get page size.");
        return Size{612,792};
    }

    return Size{ static_cast<int>(right - left), static_cast<int>(top - bottom) };
}

void Document::print_last_error() {
  unsigned long err = FPDF_GetLastError();
  fprintf(stderr, "Load pdf docs unsuccessful: ");
  switch (err) {
    case FPDF_ERR_SUCCESS:
      fprintf(stderr, "Success");
      break;
    case FPDF_ERR_UNKNOWN:
      fprintf(stderr, "Unknown error");
      break;
    case FPDF_ERR_FILE:
      fprintf(stderr, "File not found or could not be opened");
      break;
    case FPDF_ERR_FORMAT:
      fprintf(stderr, "File not in PDF format or corrupted");
      break;
    case FPDF_ERR_PASSWORD:
      fprintf(stderr, "Password required or incorrect password");
      break;
    case FPDF_ERR_SECURITY:
      fprintf(stderr, "Unsupported security scheme");
      break;
    case FPDF_ERR_PAGE:
      fprintf(stderr, "Page not found or content error");
      break;
    default:
      fprintf(stderr, "Unknown error %ld", err);
  }
  fprintf(stderr, ".\n");
}

FPDF_BOOL Document::is_data_avail(FX_FILEAVAIL* avail, size_t offset, size_t size) {
  return true;
}

void Document::idler() {}

void Document::add_segment(FX_DOWNLOADHINTS* hints, size_t offset, size_t size) {}

FPDF_PAGE Document::get_page_for_index(FPDF_FORMFILLINFO* param,
                          FPDF_DOCUMENT doc,
                          int index) {
  FPDF_FORMFILLINFO_PDFiumTest* form_fill_info =
      ToPDFiumTestFormFillInfo(param);
  auto& loaded_pages = form_fill_info->loaded_pages;
  auto iter = loaded_pages.find(index);
  if (iter != loaded_pages.end())
    return iter->second.get();

  ScopedFPDFPage page(FPDF_LoadPage(doc, index));
  if (!page)
    return nullptr;

  // Mark the page as loaded first to prevent infinite recursion.
  FPDF_PAGE page_ptr = page.get();
  loaded_pages[index] = std::move(page);

  FPDF_FORMHANDLE& form_handle = form_fill_info->form_handle;
  FORM_OnAfterLoadPage(page_ptr, form_handle);
  FORM_DoPageAAction(page_ptr, form_handle, FPDFPAGE_AACTION_OPEN);
  return page_ptr;
}


bool Document::get_page_bbox(const int page_index, float *left,
    float *bottom,
    float *right,
    float *top) const {

    *left = 0.f;
    *bottom = 0.f;
    *right = 612.f;
    *top = 792.f;

    return true;

    //FPDF_PAGE page = FPDF_LoadPage(document, page_number);

    //FPDF_PAGEOBJECT page = FPDFFormObj_GetObject(form.get(), page_index);

    //float left;
    //float bottom;
    //float right;
    //float top;

    //if (!page) {
    //    FPDFPageObj_GetBounds(page, &left, &bottom, &right, &top);
    //}

    //return true;
}



void Document::ProcessPdf(const std::string &name,
                const char *buf,
                size_t len,
                const Options &options,
                const std::function<void()> &idler)
{
    m_pLoader = new pdfium::TestLoader(buf, len);

    file_access = {};
    file_access.m_FileLen = static_cast<unsigned long>(len);
    file_access.m_GetBlock = pdfium::TestLoader::GetBlock;
    file_access.m_Param = m_pLoader;

    file_avail = {};
    file_avail.version = 1;
    file_avail.IsDataAvail = is_data_avail;

    hints = {};
    hints.version = 1;
    hints.AddSegment = add_segment;

    // |pdf_avail| must outlive |doc|.
    pdf_avail = ScopedFPDFAvail(FPDFAvail_Create(&file_avail, &file_access));

    // |doc| must outlive |form_callbacks.loaded_pages|.
    //ScopedFPDFDocument doc;

    const char *password =
        options.password.empty() ? nullptr : options.password.c_str();
    _is_linearized = false;
    // if (options.use_load_mem_document)
    // {
    //     doc.reset(FPDF_LoadMemDocument(buf, len, password));
    // }
    // else
    // {
    if (FPDFAvail_IsLinearized(pdf_avail.get()) == PDF_LINEARIZED)
    {
        int avail_status = PDF_DATA_NOTAVAIL;
        doc.reset(FPDFAvail_GetDocument(pdf_avail.get(), password));
        if (doc)
        {
            while (avail_status == PDF_DATA_NOTAVAIL)
                avail_status = FPDFAvail_IsDocAvail(pdf_avail.get(), &hints);

            if (avail_status == PDF_DATA_ERROR)
            {
                fprintf(stderr, "Unknown error in checking if doc was available.\n");
                return;
            }
            avail_status = FPDFAvail_IsFormAvail(pdf_avail.get(), &hints);
            if (avail_status == PDF_FORM_ERROR ||
                avail_status == PDF_FORM_NOTAVAIL)
            {
                fprintf(stderr,
                        "Error %d was returned in checking if form was available.\n",
                        avail_status);
                return;
            }
            _is_linearized = true;
        }
    }
    else
    {
        doc.reset(FPDF_LoadCustomDocument(&file_access, password));
    }
    // }

    if (!doc)
    {
        print_last_error();
        return;
    }

    if (!FPDF_DocumentHasValidCrossReferenceTable(doc.get()))
        fprintf(stderr, "Document has invalid cross reference table\n");

    (void)FPDF_GetDocPermissions(doc.get());

    //if (options.show_metadata)
    //    DumpMetaData(doc.get());

#ifdef PDF_ENABLE_V8
    IPDF_JSPLATFORM platform_callbacks = {};
    platform_callbacks.version = 3;
    platform_callbacks.app_alert = ExampleAppAlert;
    platform_callbacks.app_beep = ExampleAppBeep;
    platform_callbacks.app_response = ExampleAppResponse;
    platform_callbacks.Doc_getFilePath = ExampleDocGetFilePath;
    platform_callbacks.Doc_mail = ExampleDocMail;
    platform_callbacks.Doc_print = ExampleDocPrint;
    platform_callbacks.Doc_submitForm = ExampleDocSubmitForm;
    platform_callbacks.Doc_gotoPage = ExampleDocGotoPage;
    platform_callbacks.Field_browse = ExampleFieldBrowse;
#endif // PDF_ENABLE_V8

    form_callbacks = {};
#ifdef PDF_ENABLE_XFA
    form_callbacks.version = 2;
    form_callbacks.xfa_disabled =
        options.disable_xfa || options.disable_javascript;
    form_callbacks.FFI_PopupMenu = ExamplePopupMenu;
#else  // PDF_ENABLE_XFA
    form_callbacks.version = 1;
#endif // PDF_ENABLE_XFA
    form_callbacks.FFI_GetPage = get_page_for_index;

#ifdef PDF_ENABLE_V8
    if (!options.disable_javascript)
        form_callbacks.m_pJsPlatform = &platform_callbacks;
#endif // PDF_ENABLE_V8

    form = ScopedFPDFFormHandle(
        FPDFDOC_InitFormFillEnvironment(doc.get(), &form_callbacks));
    form_callbacks.form_handle = form.get();

#ifdef PDF_ENABLE_XFA
    if (!options.disable_xfa && !options.disable_javascript)
    {
        int doc_type = FPDF_GetFormType(doc.get());
        if (doc_type == FORMTYPE_XFA_FULL || doc_type == FORMTYPE_XFA_FOREGROUND)
        {
            if (!FPDF_LoadXFA(doc.get()))
                fprintf(stderr, "LoadXFA unsuccessful, continuing anyway.\n");
        }
    }
#endif // PDF_ENABLE_XFA

    FPDF_SetFormFieldHighlightColor(form.get(), FPDF_FORMFIELD_UNKNOWN, 0xFFE4DD);
    FPDF_SetFormFieldHighlightAlpha(form.get(), 100);
    FORM_DoDocumentJSAction(form.get());
    FORM_DoDocumentOpenAction(form.get());

#if _WIN32
    if (_options.output_format == OutputFormat::kPs2)
        FPDF_SetPrintMode(FPDF_PRINTMODE_POSTSCRIPT2);
    else if (options.output_format == OutputFormat::kPs3)
        FPDF_SetPrintMode(FPDF_PRINTMODE_POSTSCRIPT3);
#endif

    _page_count = FPDF_GetPageCount(doc.get());
    // int processed_pages = 0;
    // int bad_pages = 0;
    // int first_page = options.pages ? options.first_page : 0;
    // int last_page = options.pages ? options.last_page + 1 : page_count;
    // for (int i = first_page; i < last_page; ++i)

    FORM_DoDocumentAAction(form.get(), FPDFDOC_AACTION_WC);
    idler();

    // TEMP: LOAD FIRST PAGE IMMEDIATELY
    if (_is_linearized) {
        int avail_status = PDF_DATA_NOTAVAIL;
        while (avail_status == PDF_DATA_NOTAVAIL)
            avail_status = FPDFAvail_IsPageAvail(pdf_avail.get(), 0, &hints);

        if (avail_status == PDF_DATA_ERROR) {
            fprintf(stderr, "Unknown error in checking if page %d is available.\n",
                0);
            return;// std::vector<uint8_t>();
        }
    }

    //std::vector<uint8_t> result;
    //ProcessPage(_name, doc.get(), form.get(), &form_callbacks, 0, _options, result, idler);
    //g_pg1_img_buf = result;

    //std::vector<uint8_t> result2;
    //ProcessPage(_name, doc.get(), form.get(), &form_callbacks, 1, _options, result, idler);
    //g_pg2_img_buf = result2;

}

// Note, for a client using progressive rendering you'd want to determine if you
// need the rendering to pause instead of always saying |true|. This is for
// testing to force the renderer to break whenever possible.
FPDF_BOOL Document::need_to_pause_now(IFSDK_PAUSE* p) {
    return true;
}


bool Document::ProcessPage(const std::string &name,
                           FPDF_DOCUMENT doc,
                           FPDF_FORMHANDLE form,
                           FPDF_FORMFILLINFO_PDFiumTest *form_fill_info,
                           const int page_index,
                           const Options &options,
                            std::vector<uint8_t>& img_buffer,
                           const std::function<void()> &idler)
{
     FPDF_PAGE page = get_page_for_index(form_fill_info, doc, page_index);
     if (!page)
         return false;

     ScopedFPDFTextPage text_page(FPDFText_LoadPage(page));
     double scale = 1.0;
     if (!options.scale_factor_as_string.empty())
         std::stringstream(options.scale_factor_as_string) >> scale;

     auto width = static_cast<int>(FPDF_GetPageWidthF(page) * scale);
     auto height = static_cast<int>(FPDF_GetPageHeightF(page) * scale);
     int alpha = FPDFPage_HasTransparency(page) ? 1 : 0;
     ScopedFPDFBitmap bitmap(FPDFBitmap_Create(width, height, alpha));

     if (bitmap)
     {
         FPDF_DWORD fill_color = alpha ? 0x00000000 : 0xFFFFFFFF;
         FPDFBitmap_FillRect(bitmap.get(), 0, 0, width, height, fill_color);

         int flags = PageRenderFlagsFromOptions(options);

         IFSDK_PAUSE pause;
         pause.version = 1;
         pause.NeedToPauseNow = &need_to_pause_now;

         //// Client programs will be setting these values when rendering.
         //// This is a sample color scheme with distinct colors.
         //// Used only when |options.forced_color| is true.
         //const FPDF_COLORSCHEME color_scheme{
         //    /*path_fill_color=*/0xFFFF0000, /*path_stroke_color=*/0xFF00FF00,
         //    /*text_fill_color=*/0xFF0000FF, /*text_stroke_color=*/0xFF00FFFF};

         //int rv = FPDF_RenderPageBitmapWithColorScheme_Start(
         //    bitmap.get(), page, 0, 0, width, height, 0, flags,
         //    options.forced_color ? &color_scheme : nullptr, &pause);

         int rv = FPDF_RenderPageBitmapWithColorScheme_Start(
             bitmap.get(), page, 0, 0, width, height, 0, flags,
             nullptr, &pause);

         while (rv == FPDF_RENDER_TOBECONTINUED)
             rv = FPDF_RenderPage_Continue(page, &pause);

         FPDF_FFLDraw(form, bitmap.get(), page, 0, 0, width, height, 0, flags);
         idler();

         FPDF_RenderPage_Close(page);
         idler();

         int stride = FPDFBitmap_GetStride(bitmap.get());
         void *buffer = FPDFBitmap_GetBuffer(bitmap.get());

         std::string image_file_name;
         std::vector<uint8_t> image_buffer;

         switch (options.output_format)
         {
 #ifdef _WIN32
         case OutputFormat::kBmp:
             //image_file_name =
             //    WriteBmp(name.c_str(), page_index, buffer, stride, width, height);
             break;

         case OutputFormat::kEmf:
             //WriteEmf(page, name.c_str(), page_index);
             break;

         case OutputFormat::kPs2:
         case OutputFormat::kPs3:
             //WritePS(page, name.c_str(), page_index);
             break;
 #endif
         case OutputFormat::kText:
             //WriteText(text_page.get(), name.c_str(), page_index);
             break;

         case OutputFormat::kAnnot:
             //WriteAnnot(page, name.c_str(), page_index);
             break;

         case OutputFormat::kPng:

            img_buffer = 
                EncodePng(static_cast<uint8_t*>(buffer), width, height, stride, FPDFBitmap_BGRA);
             //image_file_name =
             //    WritePng(name.c_str(), page_index, buffer, stride, width, height);
             break;

         case OutputFormat::kPpm:
             img_buffer =
                 format_as_ppm(page_index, buffer, stride, width, height);
             break;

         case OutputFormat::kBgra32:
             img_buffer = 
                format_as_BGRA32(buffer, stride, width, height);
             break;
         case OutputFormat::kBgr24:
             img_buffer =
                 format_as_BGR24(buffer, stride, width, height);
             break;
         case OutputFormat::kRgb24:
             img_buffer =
                 format_as_RGB24(buffer, stride, width, height);
             break;
#ifdef PDF_ENABLE_SKIA
         case OutputFormat::kSkp:
         {
             //std::unique_ptr<SkPictureRecorder> recorder(
             //    reinterpret_cast<SkPictureRecorder *>(
             //        FPDF_RenderPageSkp(page, width, height)));
             //FPDF_FFLRecord(form, recorder.get(), page, 0, 0, width, height, 0, 0);
             //image_file_name = WriteSkp(name.c_str(), page_index, recorder.get());
         }
         break;
 #endif
         default:
             break;
         }
     }
     else
     {
         fprintf(stderr, "Page was too large to be rendered.\n");
     }

     FORM_DoPageAAction(page, form, FPDFPAGE_AACTION_CLOSE);
     idler();

     FORM_OnBeforeClosePage(page, form);
     idler();

     return !!bitmap;
}

std::vector<uint8_t> Document::load_page_ppm(int page_index, float scale) {

    if (_is_linearized) {
        int avail_status = PDF_DATA_NOTAVAIL;
        while (avail_status == PDF_DATA_NOTAVAIL)
            avail_status = FPDFAvail_IsPageAvail(pdf_avail.get(), page_index, &hints);

        if (avail_status == PDF_DATA_ERROR) {
            fprintf(stderr, "Unknown error in checking if page %d is available.\n",
                    page_index);
            return std::vector<uint8_t>();
        }
    }

    // TODO: Convert to float (not string as this is no longer coming from command line.
    _options.scale_factor_as_string = std::to_string(scale);
    std::vector<uint8_t> result;
    ProcessPage(_name, doc.get(), form.get(), &form_callbacks, page_index, _options, result, idler);

    return result;
    // if (ProcessPage(_name, doc.get(), form.get(), &form_callbacks, page_index, _options, idler)) {
    //     ++processed_pages;
    // }
    // else {
    //     ++bad_pages;
    // }
    // idler();


    // fprintf(stderr, "Processed %d pages.\n", processed_pages);
    // if (bad_pages)
    //     fprintf(stderr, "Skipped %d bad pages.\n", bad_pages);

}

}  // namespace PdfViewer::Viewer
