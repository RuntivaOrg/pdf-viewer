// Copyright 2019 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "pdfium/file_access_test_loader.h"

#include <string.h>
#include <cassert>

//#include "third_party/base/notreached.h"
namespace pdfium {
TestLoader::TestLoader(const char* buf, size_t len) : m_buf(buf), m_len(len) {}

// static
int TestLoader::GetBlock(void* param,
                         unsigned long pos,
                         unsigned char* pBuf,
                         unsigned long size) {
  TestLoader* pLoader = static_cast<TestLoader*>(param);
  if (pos + size < pos || pos + size > pLoader->m_len) {
    assert(false); // NOTREACHED();
    return 0;
  }

  memcpy(pBuf, &pLoader->m_buf[pos], size);
  return 1;
}

} // namespace pdfium
