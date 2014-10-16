#include "core\EulerAngles.h"
#include "core\OutStream.h"
#include "core\InStream.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

void EulerAngles::setFromQuaternion( const Quaternion& q )
{
   const float qx = q[0];
   const float qy = q[1];
   const float qz = q[2];
   const float qw = q[3];

   const float ww = qw*qw;
   const float xx = qx*qx;
   const float yy = qy*qy;
   const float zz = qz*qz;
   const float xy = qx*qy;
   const float zw = qz*qw;
   const float yw = qy*qw;
   const float yz = qy*qz;
   const float xw = qx*qw;
   const float xz = qx*qz;

   const float polarityTolerance = 1e-5f;

   float polarityFactor = yw - xz;
   float yaw, pitch, roll;
   if ( 0.5f - polarityFactor <= polarityTolerance )
   {
      // singularity at south pole
      roll = atan2( qx , qw );
      yaw = (float)M_PI / 2.0f;
      pitch = 0.0f;
   }
   else if ( polarityFactor + 0.5f <= polarityTolerance )
   {
      // singularity at north pole
      roll = atan2( qx , qw );
      yaw = (float)-M_PI / 2.0f;
      pitch = 0.0f;
   }
   else
   {

      // pitch works fine in range <-180, 180 >, but yaw and roll don't
      pitch = atan2( 2.0f * ( xw + yz ), 1.0f - 2.0f * ( xx + yy ) );  // theta1
      yaw = asin( 2.0f * polarityFactor ); // theta2
      roll = atan2( 2.0f * ( zw + xy ), 1.0f - 2.0f * ( yy + zz ) ); // theta3
   }

   set( RAD2DEG( yaw ), RAD2DEG( pitch ), RAD2DEG( roll ) );
}

///////////////////////////////////////////////////////////////////////////////

void EulerAngles::setFromShortestRotation( const Vector& vec1, const Vector& vec2 )
{
   ASSERT_MSG( vec1.isNormalized(), "vec1 needs to be normalized in order for setFromShortestRotation to work" );
   ASSERT_MSG( vec2.isNormalized(), "vec2 needs to be normalized in order for setFromShortestRotation to work" );

   FastFloat angle = vec1.dot( vec2 );
   angle.acos();

   FastFloat negPolarHalfAngle;
   negPolarHalfAngle.setSub( angle, Float_PI );
   
   angle.abs();
   negPolarHalfAngle.abs();

   Vector axis;

   if( angle < Float_1e_4 )
   {
      angle = Float_0;
      axis = Quad_0010;
   }
   else if ( negPolarHalfAngle < Float_1e_3 )
   { 
      axis.setCross( vec1, vec2 );
      axis.normalize();
   }
   else
   {
      axis.setCross( vec1, vec2 );
      axis.normalize();
   }

   Quaternion quat;
   quat.setAxisAngle( axis, angle );
   setFromQuaternion( quat );
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& stream, const EulerAngles& rhs)
{
   stream << "<"  << rhs[0]
          << ", " << rhs[1]
          << ", " << rhs[2]
          << ">";
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& serializer, const EulerAngles& rhs )
{
   serializer << rhs[0];
   serializer << rhs[1];
   serializer << rhs[2];
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& serializer, EulerAngles& rhs )
{
   serializer >> rhs[0];
   serializer >> rhs[1];
   serializer >> rhs[2];
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////
