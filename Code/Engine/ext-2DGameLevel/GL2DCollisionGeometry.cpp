#include "ext-2DGameLevel\GL2DCollisionGeometry.h"
#include "core\InStream.h"
#include "core\OutStream.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GL2DCollisionGeometry );
   PARENT( Component );
   PROPERTY( Array< Vector >, m_vertices );
   PROPERTY( Array< GL2DCollisionTriangle >, m_faces );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

GL2DCollisionGeometry::GL2DCollisionGeometry( const char* name )
   : Component( name )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DCollisionGeometry::GL2DCollisionGeometry( const GL2DCollisionGeometry& rhs )
   : Component( rhs )
   , m_vertices( rhs.m_vertices )
   , m_faces( rhs.m_faces )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DCollisionGeometry::~GL2DCollisionGeometry()
{
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionGeometry::setGeometry( const Array< Vector >& vertices, const Array< GL2DCollisionTriangle >& faces, const Matrix& transform )
{
   // copy & transform the vertices
   uint count = vertices.size();
   m_vertices.resizeWithoutInitializing( count );
   for ( uint i = 0; i < count; ++i )
   {
      transform.transform( vertices[i], m_vertices[i] );
   }

   // copy the faces
   m_faces = faces;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GL2DCollisionTriangle::GL2DCollisionTriangle()
{
   idx[0] = idx[1] = idx[2] = -1;
}

///////////////////////////////////////////////////////////////////////////////

GL2DCollisionTriangle::GL2DCollisionTriangle( word i0, word i1, word i2 )
{
   idx[0] = i0; 
   idx[1] = i1; 
   idx[2] = i2;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DCollisionTriangle::set( word i0, word i1, word i2 )
{
   idx[0] = i0; 
   idx[1] = i1; 
   idx[2] = i2;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const GL2DCollisionTriangle& face )
{
   stream << face.idx[0];
   stream << face.idx[1];
   stream << face.idx[2];
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, GL2DCollisionTriangle& face )
{
   stream >> face.idx[0];
   stream >> face.idx[1];
   stream >> face.idx[2];
   return stream;
}

///////////////////////////////////////////////////////////////////////////////
