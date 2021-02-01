#ifndef PdfViewer_Macros_H
#define PdfViewer_Macros_H
#pragma once

// Pragmas removed from linux
//-Wlanguage-extension-token
//-Wgnu-anonymous-struct
/*
_Pragma("GCC diagnostic ignored \"-Wgnu-zero-variadic-macro-arguments\"") \
_Pragma("GCC diagnostic ignored \"-Wimplicit-int-float-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wimplicit-float-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wfloat-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wnested-anon-types\"") \
_Pragma("GCC diagnostic ignored \"-Wsign-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wmicrosoft-enum-value\"") \
_Pragma("GCC diagnostic ignored \"-Wimplicit-int-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wshorten-64-to-32\"") \
_Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
_Pragma("GCC diagnostic ignored \"-Wunused-function\"") \
_Pragma("GCC diagnostic ignored \"-Wextra-semi\"")              
*/

#ifdef _WIN32
#define DISABLE_WARNINGS \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wall\"") \
_Pragma("GCC diagnostic ignored \"-Wextra\"") \
_Pragma("GCC diagnostic ignored \"-Wlanguage-extension-token\"") \
_Pragma("GCC diagnostic ignored \"-Wgnu-anonymous-struct\"") \
_Pragma("GCC diagnostic ignored \"-Wgnu-zero-variadic-macro-arguments\"") \
_Pragma("GCC diagnostic ignored \"-Wimplicit-int-float-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wimplicit-float-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wfloat-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wnested-anon-types\"") \
_Pragma("GCC diagnostic ignored \"-Wsign-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wmicrosoft-enum-value\"") \
_Pragma("GCC diagnostic ignored \"-Wimplicit-int-conversion\"") \
_Pragma("GCC diagnostic ignored \"-Wshorten-64-to-32\"") \
_Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
_Pragma("GCC diagnostic ignored \"-Wunused-function\"") \
_Pragma("GCC diagonstic ignored \"-Wreserved-id-macro\"") \
_Pragma("GCC diagnostic ignored \"-Wextra-semi\"")              
#else 
#define DISABLE_WARNINGS \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wreserved-id-macro\"") \
_Pragma("GCC diagnostic ignored \"-Wall\"") \
_Pragma("GCC diagnostic ignored \"-Wextra\"") \
_Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
_Pragma("GCC diagnostic ignored \"-Wattributes\"") \

#endif // __linux__ or _WIN32

#define ENABLE_WARNINGS _Pragma("GCC diagnostic pop")

#endif  // !PdfViewer_Macros_H

