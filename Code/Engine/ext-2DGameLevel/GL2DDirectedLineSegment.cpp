#include "ext-2DGameLevel\GL2DDirectedLineSegment.h"
#include "core\InStream.h"
#include "core\OutStream.h"
#include "core\VectorUtil.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

GL2DDirectedLineSegment::GL2DDirectedLineSegment()
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DDirectedLineSegment::GL2DDirectedLineSegment( const Vector& start, const Vector& end )
: m_start( start )
, m_end( end )
{
   calcInternals();
}

///////////////////////////////////////////////////////////////////////////////

GL2DDirectedLineSegment::GL2DDirectedLineSegment( const GL2DDirectedLineSegment& rhs )
: m_start( rhs.m_start )
, m_end( rhs.m_end )
, m_length( rhs.m_length )
, m_direction( rhs.m_direction )
, m_normal( rhs.m_normal )
{
}

///////////////////////////////////////////////////////////////////////////////

void GL2DDirectedLineSegment::set( const Vector& start, const Vector& end )
{
   m_start = start;
   m_end = end;

   calcInternals();
}

///////////////////////////////////////////////////////////////////////////////

void GL2DDirectedLineSegment::setTransformed( const Matrix& transform, const GL2DDirectedLineSegment& lhs )
{
   transform.transform( lhs.m_start, m_start );
   transform.transform( lhs.m_end, m_end );
   calcInternals();
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const GL2DDirectedLineSegment& segment )
{
   stream << segment.m_start;
   stream << segment.m_end;

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, GL2DDirectedLineSegment& segment )
{
   stream >> segment.m_start;
   stream >> segment.m_end;

   segment.calcInternals();

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DDirectedLineSegment::calcInternals()
{
   m_direction.setSub( m_end, m_start );
   VectorUtil::calculatePerpendicularVector2D( m_direction, m_normal );
   m_length = m_direction.length();
   m_direction.div( m_length );
   m_normal.normalize();
}

///////////////////////////////////////////////////////////////////////////////
