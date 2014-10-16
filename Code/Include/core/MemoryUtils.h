/// @file   core/MemoryUtils.h
/// @brief  memory management utility functions
#ifndef _MEMORY_UTILS_H
#define _MEMORY_UTILS_H

#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Memory management utility functions.
 */
class MemoryUtils
{
public:
   /**
    * Aligns a pointer using the specified alignment.
    *
    * @param ptr           original, unaligned pointer
    * @return              aligned pointer
    */
   static inline void* alignAddress( void* ptr );

   /**
    * Aligns a pointer using the specified alignment and stores the original address
    * 4 bytes before the returned pointer.
    *
    * @param ptr           original, unaligned pointer
    * @return              aligned pointer
    */
   static inline void* alignAddressAndStoreOriginal( void* ptr );

   /**
    * Reverse-aligns a pointer aligned with 'alignAddressAndStoreOriginal' method.
    *
    * @param   alignedPtr
    * @return  original, unaligned pointer
    */
   static inline void* resolveAlignedAddress( void* alignedPtr );

   /**
    * Checks if the specified address is aligned to the specified boundary.
    *
    * @param ptr
    */
   static inline bool isAddressAligned( void* ptr );

   /**
    * Calculates the amount of memory needed to store an aligned version of an object with the specified size
    * using the specified alignment.
    *
    * @param size
    */
   static inline size_t calcAlignedSize( size_t size );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\MemoryUtils.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _MEMORY_UTILS_H
