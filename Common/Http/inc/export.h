/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include <memory>
#include <qglobal.h>

#if defined(Q_OS_WIN)
    #if defined(HTTPLIB_EXPORT)
        #define HTTP_API Q_DECL_EXPORT
    #elif defined(HTTPLIB_IMPORT_IMPORT)
        #define HTTP_API Q_DECL_IMPORT
    #endif
#endif

#if !defined(HTTP_API)
    #define HTTP_API
#endif
