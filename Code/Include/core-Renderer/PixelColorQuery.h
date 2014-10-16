/// @file   core-Renderer/PixelColorQuery.h
/// @brief  this structure is used during an RCGetPixel query to store query results
#pragma once

#include "core\ReflectionObject.h"
#include "core\MemoryRouter.h"
#include "core\Vector.h"
#include "core\Color.h"


/////////////////////////////////////////////////////////////////////////////// 

class PixelColorQuery
{
   DECLARE_ALLOCATOR( PixelColorQuery, AM_ALIGNED_16 );

public:
   bool        m_isCompleted;

   Vector      m_queryPos;
   Color       m_outColorValue;

public:
   /**
    * Constructor.
    */
   PixelColorQuery();
   ~PixelColorQuery();
};

/////////////////////////////////////////////////////////////////////////////// 

class SurfaceQuery
{
   DECLARE_ALLOCATOR( SurfaceQuery, AM_DEFAULT );

public:
   int         m_snapshotIdx;

   byte*       m_buffer;
   uint        m_size;

   uint        m_width;
   uint        m_height;
   byte        m_pixelStride;

public:
   /**
    * Constructor.
    */
   SurfaceQuery();
   ~SurfaceQuery();

   /**
    * Allocates/reallocates the buffer, if needed.
    *
    * @param width
    * @param height
    * @param pixelStride
    */
   void allocateBuffer( uint width, uint height, byte pixelStride );

   /**
    * Resets contents of the buffer, without deleting it.
    */
   void resetContents();
};

///////////////////////////////////////////////////////////////////////////////
