// Copyright 2018 The PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PDF_TEST_WRITE_HELPER_H_
#define PDF_TEST_WRITE_HELPER_H_

#include <vector>

#include "macros.h"
DISABLE_WARNINGS
#pragma GCC diagnostic ignored "-Wgnu-statement-expression"
#pragma GCC diagnostic ignored "-Wnonportable-system-include-path"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "third_party/base/span.h"
ENABLE_WARNINGS

#ifdef PDF_ENABLE_SKIA
#include "third_party/skia/include/core/SkPictureRecorder.h"  // nogncheck
#include "third_party/skia/include/core/SkStream.h"  // nogncheck
#endif

bool _check_dimensions(int stride, int width, int height);

std::vector<uint8_t> EncodePng(const uint8_t* input,
   int width,
   int height,
   int stride,
   int format);

std::vector<uint8_t> format_as_BGRA32(
    void* buffer_void,
    int stride,
    int width,
    int height);

std::vector<uint8_t> format_as_RGB24(
    void* buffer_void,
    int stride,
    int width,
    int height);

std::vector<uint8_t> format_as_BGR24(
    void* buffer_void,
    int stride,
    int width,
    int height);

std::vector<uint8_t> format_as_ppm(
    int page_index,
    void *buffer_void,
    int stride,
    int width,
    int height);

#endif // PDF_TEST_WRITE_HELPER_H_
