#include "pdfium/file_access_for_testing.h"

#include <stdio.h>
#include <string.h>

//#include "testing/utils/path_service.h"

// commented the following while adding the test dependencies into this target

// std::unique_ptr<char, pdfium::FreeDeleter> GetFileContents(const char* filename,
//                                                            size_t* retlen) {
//   FILE* file = fopen(filename, "rb");
//   if (!file) {
//     fprintf(stderr, "Failed to open: %s\n", filename);
//     return nullptr;
//   }
//   (void)fseek(file, 0, SEEK_END);
//   size_t file_length = static_cast<size_t>(ftell(file));
//   if (!file_length) {
//     return nullptr;
//   }
//   (void)fseek(file, 0, SEEK_SET);
//   std::unique_ptr<char, pdfium::FreeDeleter> buffer(
//       static_cast<char*>(malloc(file_length)));
//   if (!buffer) {
//     return nullptr;
//   }
//   size_t bytes_read = fread(buffer.get(), 1, file_length, file);
//   (void)fclose(file);
//   if (bytes_read != file_length) {
//     fprintf(stderr, "Failed to read: %s\n", filename);
//     return nullptr;
//   }
//   *retlen = bytes_read;
//   return buffer;
// }
