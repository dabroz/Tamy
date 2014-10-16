/// @file   core/EngineDefines.h
/// @brief  here's where all major engine-level macros and definitions are stored
#ifndef _ENGINE_DEFINES_H
#define _ENGINE_DEFINES_H


///////////////////////////////////////////////////////////////////////////////

/**
 * Toggles fast memory management use in Arrays
 */
#define _USE_FAST_ARRAYS

/**
 * Toggles global allocations tracing - each time global new/delete operators
 * will be used, a message will be logged.
 */
// #define _TRACE_GLOBAL_ALLOCATIONS

/**
 * Enables memory allocations tracking - useful to track memory leaks
 */
// #define _TRACK_MEMORY_ALLOCATIONS

/**
 * Enables the 'ClientServerRoundBuffer' debugging - enable this if you're running
 * into fencing problems on the rendering thread for instance.
 */
// #define _DEBUG_ROUNDBUFFER

///////////////////////////////////////////////////////////////////////////////

#define _USE_SIMD

#define SSE_VERSION 0x20

#ifdef _USE_SIMD
   // include SSE
   #include <xmmintrin.h>

   #if SSE_VERSION >= 0x20
   #include <emmintrin.h>
   #endif

   #if SSE_VERSION >= 0x30
   #include <pmmintrin.h>
   #endif

   #if SSE_VERSION >= 0x31
   #include <tmmintrin.h>
   #endif

   #if SSE_VERSION >= 0x40
   #include <wmmintrin.h>
   #endif

   #if SSE_VERSION >= 0x41
   #include <smmintrin.h>
   #endif

   #if SSE_VERSION >= 0x42
   #include <nmmintrin.h>
   #endif

   #if SSE_VERSION >= 0x50
   #include <immintrin.h>
   #endif
#endif // _USE_SIMD

///////////////////////////////////////////////////////////////////////////////

/**
 * This macro allows to embed a code that will be executed only in the debug version
 */
#ifdef _DEBUG
   #define DEBUG_CODE( Code )  { Code }
#else
   #define DEBUG_CODE( Code )
#endif

///////////////////////////////////////////////////////////////////////////////

#endif // _ENGINE_DEFINES_H 
