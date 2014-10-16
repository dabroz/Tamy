/// @file   core/ReflectionSerializationMacros.h
/// @brief  a couple of macros used during ReflectionObjects serialization
#pragma once


///////////////////////////////////////////////////////////////////////////////

#define TAMY_ARCHIVE_MAGIC_NO          0xABCD

///////////////////////////////////////////////////////////////////////////////

#define EXTERNAL_DEPENDENCY_MARKER     0x8000000
#define MAX_EXTERNAL_DEPENDENCIES      1024

///////////////////////////////////////////////////////////////////////////////

#define CREATE_EXTERNAL_DEPENDENCY_INDEX( dependencyIdx )    ( ( dependencyIdx + 1 ) == 0 ? 0 : ( EXTERNAL_DEPENDENCY_MARKER | ( dependencyIdx + 1 ) ) )

#define CREATE_INTERNAL_DEPENDENCY_INDEX( dependencyIdx )    ( dependencyIdx + 1 )

///////////////////////////////////////////////////////////////////////////////

#define EXTRACT_DEPENDENCY_INDEX( compressedIdx, outDependencyIdx, outIsExternal ) \
   { \
      outIsExternal = ( compressedIdx & EXTERNAL_DEPENDENCY_MARKER ) == EXTERNAL_DEPENDENCY_MARKER; \
      if ( outIsExternal ) \
      { \
         outDependencyIdx = ( compressedIdx ^ EXTERNAL_DEPENDENCY_MARKER ) - 1; \
         if ( outDependencyIdx > MAX_EXTERNAL_DEPENDENCIES ) \
         { \
            outIsExternal = false; \
            outDependencyIdx = -1; \
         } \
      } else { \
         outDependencyIdx = compressedIdx - 1; \
      } \
   }

///////////////////////////////////////////////////////////////////////////////

#define IS_DEPENDENCY_NULL( dependencyIdx ) ( dependencyIdx == 0 ) 

///////////////////////////////////////////////////////////////////////////////
