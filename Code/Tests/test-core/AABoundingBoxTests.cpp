#include "core-TestFramework\TestFramework.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

TEST( AxisAlignedBox, transformation )
{
   AxisAlignedBox centralBB( Vector( -1, -1, -1 ), Vector( 1, 1, 1 ) );

   Transform transform;
   Matrix transformMtx;
   AxisAlignedBox transformedBB;

   {
      transform.m_rotation.setAxisAngle( Vector_OZ, FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );
      transform.toMatrix( transformMtx );
      centralBB.transform( transformMtx, transformedBB );

      COMPARE_VEC( Vector( -1.4142135f, -1.4142135f, -1 ), transformedBB.min );
      COMPARE_VEC( Vector(  1.4142135f,  1.4142135f,  1 ), transformedBB.max );
   }

   {
      transform.m_rotation.setAxisAngle( Vector_OX, FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );
      transform.toMatrix( transformMtx );
      centralBB.transform( transformMtx, transformedBB );

      COMPARE_VEC( Vector( -1, -1.4142135f, -1.4142135f ), transformedBB.min );
      COMPARE_VEC( Vector(  1,  1.4142135f,  1.4142135f ), transformedBB.max );
   }

   {
      transform.m_rotation.setAxisAngle( Vector_OY, FastFloat::fromFloat( DEG2RAD( 45.0f ) ) );
      transform.toMatrix( transformMtx );
      centralBB.transform( transformMtx, transformedBB );

      COMPARE_VEC( Vector( -1.4142135f, -1, -1.4142135f ), transformedBB.min );
      COMPARE_VEC( Vector(  1.4142135f,  1,  1.4142135f ), transformedBB.max );
   }
}

///////////////////////////////////////////////////////////////////////////////
