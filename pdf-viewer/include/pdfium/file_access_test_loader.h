// Copyright 2019 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FILE_ACCESS_TEST_LOADER_H_
#define FILE_ACCESS_TEST_LOADER_H_

#include <stddef.h>
//#include "third_party/base/span.h"

namespace pdfium {

class TestLoader {
 public:
  explicit TestLoader(const char* buf, size_t len);

  static int GetBlock(void* param,
                      unsigned long pos,
                      unsigned char* pBuf,
                      unsigned long size);

 private:
  const char* m_buf;
  const size_t m_len; 
  //const pdfium::span<const char> m_Span;
};
} // namespace pdfium
#endif  // FILE_ACCESS_TEST_LOADER_H_
