#include "core\Plane.h"
#include "core\Vector.h"
#include "core\OutStream.h"
#include "core\InStream.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

void Plane::setFromPointNormal( const Vector& point, const Vector& normal )
{
   normal.store( m_quad );
   FastFloat dot;
   dot.setNeg( normal.dot( point ) );
   (*this)[3] = dot.getFloat();
}

///////////////////////////////////////////////////////////////////////////////

void Plane::setFromPoints( const Vector& p1, const Vector& p2, const Vector& p3 )
{
   Vector edge1, edge2;
   edge1.setSub( p2, p1 );
   edge2.setSub( p3, p1 );

   Vector normal;
   normal.setCross( edge1, edge2 );
   normal.normalize();

   normal.store( m_quad );
   FastFloat dot;
   dot.setNeg( normal.dot( p1 ) );
   (*this)[3] = dot.getFloat();
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<( std::ostream& stream, const Plane& rhs )
{
   stream << "[" << rhs[0] << ", " << rhs[1] << ", " << rhs[2] << ", " << rhs[3] << "]";
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& serializer, const Plane& rhs )
{
   serializer << rhs[0];
   serializer << rhs[1];
   serializer << rhs[2];
   serializer << rhs[3];
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& serializer, Plane& rhs )
{
   serializer >> rhs[0];
   serializer >> rhs[1];
   serializer >> rhs[2];
   serializer >> rhs[3];
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////
