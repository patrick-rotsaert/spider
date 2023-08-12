//
// Copyright (C) 2023 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

// clang-format off

#if defined _WIN32 || defined __CYGWIN__
  #define SPIDER_API_IMPORT __declspec(dllimport)
  #define SPIDER_API_EXPORT __declspec(dllexport)
  #define SPIDER_API_LOCAL
#else
  #if __GNUC__ >= 4 // TODO: clang?
    #define SPIDER_API_IMPORT __attribute__ ((visibility ("default")))
    #define SPIDER_API_EXPORT __attribute__ ((visibility ("default")))
    #define SPIDER_API_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define SPIDER_API_IMPORT
    #define SPIDER_API_EXPORT
    #define SPIDER_API_LOCAL
  #endif
#endif

#ifdef SPIDER_SHARED // compiled as a shared library
  #ifdef SPIDER_SHARED_EXPORTS // defined if we are building the shared library
    #define SPIDER_EXPORT SPIDER_API_EXPORT
  #else
    #define SPIDER_EXPORT SPIDER_API_IMPORT
  #endif // SPIDER_SHARED_EXPORTS
  #define SPIDER_LOCAL SPIDER_API_LOCAL
#else // compiled as a static library
  #define SPIDER_EXPORT
  #define SPIDER_LOCAL
#endif // SPIDER_SHARED
