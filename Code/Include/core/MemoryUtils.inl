#ifndef _MEMORY_UTILS_H
#error "This file can only be included in MemoryUtils.h"
#else

#include <memory>

#define RAW_ALIGNMENT_VALUE 16  
#define ALIGNMENT 19                   // align_16 + sizeof( void* )  - 1 = 16 + 4 - 1 = 19
#define NOT_ALIGNMENT (ulong)~15       // align_16 - 1

///////////////////////////////////////////////////////////////////////////////

void* MemoryUtils::alignAddress( void* ptr )
{
   void* alignedPtr = ( void* ) ( ( ( ulong ) ptr + ALIGNMENT ) & NOT_ALIGNMENT );
   return alignedPtr;
}

///////////////////////////////////////////////////////////////////////////////

void* MemoryUtils::alignAddressAndStoreOriginal( void* ptr )
{
   void* alignedPtr = ( void* ) ( ( ( ulong ) ptr + ALIGNMENT ) & NOT_ALIGNMENT );

   *( ( void ** ) alignedPtr - 1 ) = ptr;
   return alignedPtr;
}

///////////////////////////////////////////////////////////////////////////////

void* MemoryUtils::resolveAlignedAddress( void* alignedPtr )
{
   if ( alignedPtr )
   {
      return *( ( void ** ) alignedPtr - 1 );
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

bool MemoryUtils::isAddressAligned( void* ptr )
{
   ulong specifiedAddr = ( ulong ) ptr;
   ulong alignedAddr = ( specifiedAddr + ALIGNMENT ) & NOT_ALIGNMENT;

   return ( ( specifiedAddr - alignedAddr ) % RAW_ALIGNMENT_VALUE ) == 0;
}

///////////////////////////////////////////////////////////////////////////////

size_t MemoryUtils::calcAlignedSize( size_t size )
{
   return size + ALIGNMENT;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _MEMORY_UTILS_H
