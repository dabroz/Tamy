#include "core.h"
#include "core/PlaneUtils.h"
#include "core/Plane.h"
#include "core/Vector.h"


///////////////////////////////////////////////////////////////////////////////

bool PlaneUtils::calculatePlanesIntersection( const Plane& p1, const Plane& p2, const Plane& p3, Vector& outIntersectionPt )
{
   Vector n1, n2, n3;
   p1.getNormal( n1 );
   p2.getNormal( n2 );
   p3.getNormal( n3 );

   Vector n23, n31, n12;
   n23.setCross( n2, n3 );
   n31.setCross( n3, n1 );
   n12.setCross( n1, n2 );

   FastFloat den = p1.dotNormal( n23 );
   den.neg();
   if ( den == Float_0 )
   {
      // planes are parallel
      return false;
   }

   Vector a, b, c;
   a.setMul( n23, p1.getComponent(3) );
   b.setMul( n31, p2.getComponent(3) );
   c.setMul( n12, p3.getComponent(3) );

   den.reciprocal();
   outIntersectionPt.setAdd( a, b );
   outIntersectionPt.add( c );
   outIntersectionPt.mul( den );
   return true;
}

///////////////////////////////////////////////////////////////////////////////
