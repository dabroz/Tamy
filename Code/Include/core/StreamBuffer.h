/// @file   core\StreamBuffer.h
/// @brief  utility for reading file contents as a c-style array
#ifndef _STREAM_BUFFER_H
#define _STREAM_BUFFER_H

#include "core\Filesystem.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This utility class provides an access to a file as if it was
 * a memory buffer.
 * It's a wrapper that takes the ownership over a file.
 */
template<typename T>
class StreamBuffer
{
   DECLARE_ALLOCATOR( StreamBuffer, AM_DEFAULT );

private:
   T* m_buffer;
   std::size_t m_size;

public:
   /**
    * Constructor.
    * 
    * @param file    file which we want to access in a buffer-like way
    */
   StreamBuffer(File& file);
   ~StreamBuffer();

   /**
    * The method returns the underlying buffer for C-style operations.
    *
    * @return  C-style memory buffer
    */
   const T* getBuffer();

   /**
    * Access operator to buffer elements.
    *
    * @param idx     index of an element we want to access
    * @return        reference to the desired buffer element
    */
   T& operator[] (unsigned int idx);

   /**
    * Access operator to buffer elements (for execution
    * from a constant context).
    *
    * @param idx     index of an element we want to access
    * @return        reference to the desired buffer element
    */
   const T& operator[] (unsigned int idx) const;

   /**
    * Returns the size of the buffer (number of its elements)
    *
    * @return     number of buffer elements
    */
   std::size_t size();
};

///////////////////////////////////////////////////////////////////////////////

#include "core\StreamBuffer.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _STREAM_BUFFER_H
