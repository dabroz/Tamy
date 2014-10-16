/// @file   core/types.h
/// @brief  basic types definitions
#pragma once


///////////////////////////////////////////////////////////////////////////////

typedef unsigned char            byte;
typedef unsigned short           word;
typedef unsigned int             uint;
typedef unsigned long            ulong;

///////////////////////////////////////////////////////////////////////////////

/**
 * Memory alignment macros.
 */
#define ALIGN_8      __declspec(align(8))
#define ALIGN_16     __declspec(align(16))
#define ALIGN_32     __declspec(align(32))
#define ALIGN_64     __declspec(align(64))
#define ALIGN_128    __declspec(align(128))

///////////////////////////////////////////////////////////////////////////////
