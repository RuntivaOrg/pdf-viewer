#include "pdfium/pdf_test_write_helper.h"

DISABLE_WARNINGS
#pragma GCC diagnostic ignored "-Wgnu-statement-expression"
#pragma GCC diagnostic ignored "-Wnonportable-system-include-path"
#include "pdfium/png_image_utils.h"
#include "public/fpdf_edit.h"
//#include "third_party/base/span.h"
#include "third_party/base/notreached.h"

ENABLE_WARNINGS
#include <string>
#include <iostream>
#include <sstream>

bool _check_dimensions(int stride, int width, int height) {
  if (stride < 0 || width < 0 || height < 0)
    return false;
  if (height > 0 && stride > INT_MAX / height)
    return false;
  return true;
}


std::vector<uint8_t> EncodePng(const uint8_t* input,
    int width,
    int height,
    int stride,
    int format) {

    auto input_span =
      pdfium::make_span(input, static_cast<size_t>(stride * height));

    std::vector<uint8_t> png;
    switch (format) {
    case FPDFBitmap_Unknown:
        break;
    case FPDFBitmap_Gray:
        png = png_image_utils::EncodeGrayPNG(input_span, width, height, stride);
        break;
    case FPDFBitmap_BGR:
        png = png_image_utils::EncodeBGRPNG(input_span, width, height, stride);
        break;
    case FPDFBitmap_BGRx:
        png = png_image_utils::EncodeBGRAPNG(input_span, width, height, stride,
            /*discard_transparency=*/true);
        break;
    case FPDFBitmap_BGRA:
        png = png_image_utils::EncodeBGRAPNG(input_span, width, height, stride,
            /*discard_transparency=*/false);
        break;
    default:
        NOTREACHED();
    }
    return png;
}


std::vector<uint8_t> format_as_BGRA32(
    void* buffer_void,
    [[maybe_unused]]int stride,
    int width,
    int height)
{
    const uint8_t* buffer = reinterpret_cast<const uint8_t*>(buffer_void);
    int size = width * height * 4;
    std::vector<uint8_t> result(buffer, buffer + size);

    return result;
}

/// Converts image buffer from 
/// B, G, R, unused
/// to 
/// R, G, B
std::vector<uint8_t> format_as_RGB24(
    void* buffer_void,
    int stride,
    int width,
    int height)
{
    if (!_check_dimensions(stride, width, height))
        return std::vector<uint8_t>();

    int out_len = width * height;
    if (out_len > INT_MAX / 3)
        return std::vector<uint8_t>();

    out_len *= 3;


    // Source data is B, G, R, unused.
    // Dest data is B, G, R.
    const uint8_t* buffer = reinterpret_cast<const uint8_t*>(buffer_void);
    std::vector<uint8_t> result(static_cast<size_t>(out_len));


    for (int h = 0; h < height; ++h)
    {
        const uint8_t* src_line = buffer + (stride * h);
        uint8_t* dest_line = result.data() + (width * h * 3);
        for (int w = 0; w < width; ++w)
        {
            // R
            dest_line[w * 3] = src_line[(w * 4) + 2];
            // G
            dest_line[(w * 3) + 1] = src_line[(w * 4) + 1];
            // B
            dest_line[(w * 3) + 2] = src_line[w * 4];
        }
    }

    return result;
}


/// Converts image buffer from 
/// B, G, R, unused
/// to 
/// B, G, R
std::vector<uint8_t> format_as_BGR24(
    void* buffer_void,
    int stride,
    int width,
    int height)
{
    if (!_check_dimensions(stride, width, height))
        return std::vector<uint8_t>();

    int out_len = width * height;
    if (out_len > INT_MAX / 3)
        return std::vector<uint8_t>();

    out_len *= 3;


    // Source data is B, G, R, unused.
    // Dest data is B, G, R.
    const uint8_t* buffer = reinterpret_cast<const uint8_t*>(buffer_void);
    std::vector<uint8_t> result(static_cast<size_t>(out_len));


    for (int h = 0; h < height; ++h)
    {
        const uint8_t* src_line = buffer + (stride * h);
        uint8_t* dest_line = result.data() + (width * h * 3);
        for (int w = 0; w < width; ++w)
        {
            // B
            dest_line[w * 3] = src_line[w * 4];
            // G
            dest_line[(w * 3) + 1] = src_line[(w * 4) + 1];
            // R
            dest_line[(w * 3) + 2] = src_line[(w * 4) + 2];
        }
    }

    return result;
}


std::vector<uint8_t> format_as_ppm(
    [[maybe_unused]]int page_index,
    void *buffer_void,
    int stride,
    int width,
    int height)
{
    if (!_check_dimensions(stride, width, height))
        return std::vector<uint8_t>();

    int out_len = width * height;
    if (out_len > INT_MAX / 3)
        return std::vector<uint8_t>();

    out_len *= 3;

    std::ostringstream stringStream;
    stringStream << "P6\n# PDF test render\n"
                 << width << " " << height << "\n255\n";

    //stringStream << "P6\n# PPM Page " << page_index << "\n"
    //    << width << " " << height << "\n255\n";
    

    std::string header = "";// stringStream.str();
    auto header_offset = header.size();

    // Source data is B, G, R, unused.
    // Dest data is R, G, B.
    const uint8_t *buffer = reinterpret_cast<const uint8_t *>(buffer_void);
    std::vector<uint8_t> result(static_cast<size_t>(out_len));


    //result.assign(header.begin(), header.end());


    for (int h = 0; h < height; ++h)
    {
        const uint8_t *src_line = buffer + (stride * h);
        uint8_t *dest_line = result.data() + /*header_offset*/ 0 + (width * h * 3);
        for (int w = 0; w < width; ++w)
        {
            // R
            dest_line[w * 3] = src_line[(w * 4) + 2];
            // G
            dest_line[(w * 3) + 1] = src_line[(w * 4) + 1];
            // B
            dest_line[(w * 3) + 2] = src_line[w * 4];
        }
    }

    std::vector<uint8_t> result2(static_cast<size_t>(out_len) + header_offset);

    size_t idx = 0;
    for (auto& s : header) {
        result2[idx++] = static_cast<unsigned char>(s);
    }
    for (auto& d : result) {
        result2[idx++] = d;
    }

    
    //char filename[256];
    //snprintf(filename, sizeof(filename), "C:\\work\\PDF\\SampleFiles\\airplane.pdf.P.ppm");
    //FILE* fp = fopen(filename, "wb");
    //if (!fp)
    //    return std::vector<uint8_t>();

    //if (fwrite(result2.data(), result2.size(), 1, fp) != 1)
    //    fprintf(stderr, "Failed to write to %s\n", filename);

    //fclose(fp);
    
//    result2.assign(result2.begin() + static_cast<int>(header_offset), result.begin(), result.end());



    return result2;
}
