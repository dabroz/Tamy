/// @file   ext-2DGameLevel\GL2DDirectedLineSegment.h
/// @brief  a line segment definition
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"
#include "core\FastFloat.h"


///////////////////////////////////////////////////////////////////////////////

class InStream;
class OutStream;
struct Matrix;

///////////////////////////////////////////////////////////////////////////////

ALIGN_16 struct GL2DDirectedLineSegment
{
   DECLARE_ALLOCATOR( GL2DDirectedLineSegment, AM_DEFAULT );

   Vector      m_start;
   Vector      m_end;

   FastFloat   m_length;
   Vector      m_direction;
   Vector      m_normal;

   /**
   * Default constructor.
   */
   GL2DDirectedLineSegment();

   /**
   * Constructor.
   *
   * @param start
   * @param end
   */
   GL2DDirectedLineSegment( const Vector& start, const Vector& end );

   /**
   * Copy constructor.
   *
   * @param rhs
   */
   GL2DDirectedLineSegment( const GL2DDirectedLineSegment& rhs );

   /**
   * Sets a new value of the line segment.
   *
   * @param start
   * @param end
   */
   void set( const Vector& start, const Vector& end );

   /**
    * Transforms the specified 'lhs' segment and initializes this one with the result.
    *
    * @param transform
    * @param lhs
    */
   void setTransformed( const Matrix& transform, const GL2DDirectedLineSegment& lhs );

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const GL2DDirectedLineSegment& segment );
   friend InStream& operator>>( InStream& stream, GL2DDirectedLineSegment& segment );

private:
   void calcInternals();
};

///////////////////////////////////////////////////////////////////////////////
