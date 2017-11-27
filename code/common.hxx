// common.hxx
// This file is included by every header and source file, and is included before
// anything else. It contains mostly preprocessor macro definitions.

#ifndef BUCKET_COMMON_HXX
#define BUCKET_COMMON_HXX

// List of Macros:
//
// Build Options:
//   BUCKET_DISABLE_COMPILER_EXTENSIONS - if this is defined, all compiler
//     specific code is disabled.
//   BUCKET_DEBUG_BUILD - if this is defined, runtime checks are enabled for
//      program invariants.
//
// Other:
//   BUCKET_RESTRICT - if compiler extensions are enabled and the compiler in
//     use has a keyword analogous to 'restrict' in C, BUCKET_RESTRICT is set to
//     that keyword. Otherwise the macro is defined but empty.

#ifndef BUCKET_DISABLE_COMPILER_EXTENSIONS
  #ifdef __clang__
    #define BUCKET_COMPILER_IS_CLANG
    #define BUCKET_RESTRICT __restrict__
  #elif defined(__GNUC__)
    #define BUCKET_COMPILER_IS_GCC
    #define BUCKET_RESTRICT __restrict__
  #elif defined(_MSC_VER)
    #define BUCKET_COMPILER_IS_MSVC
    #define BUCKET_RESTRICT __restrict
  #else
    #define BUCKET_RESTRICT
  #endif
#else
  #define BUCKET_RESTRICT
#endif

#endif
