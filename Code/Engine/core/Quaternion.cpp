#include "core\Quaternion.h"
#include "core\EulerAngles.h"
#include "core\OutStream.h"
#include "core\InStream.h"
#include "core\MathDefs.h"
#include "core\Vector.h"
#include "core\VectorUtil.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

Quaternion Quaternion::IDENTITY;

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setAxisAngle( const Vector& axis, float angle )
{
   setAxisAngle( axis, FastFloat::fromFloat( angle ) );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setFromEulerAngles( const EulerAngles& angles )
{
   // Q(Roll) * Q(Pitch) * Q(Yaw) - to be in sync with the counterpart method Matrix::setRotation( const EulerAngles& angles )

   const double radYaw = DEG2RAD( angles[ EulerAngles::Yaw ] * 0.5f );
   const double radPitch = DEG2RAD( angles[ EulerAngles::Pitch ] * 0.5f );
   const double radRoll = DEG2RAD( angles[ EulerAngles::Roll ] * 0.5f );

   double c1 = cos( radYaw );
   double s1 = sin( radYaw );
   double c2 = cos( radRoll );
   double s2 = sin( radRoll );
   double c3 = cos( radPitch );
   double s3 = sin( radPitch );

   double c2s3 = c2*s3;
   double c2c3 = c2*c3;
   double s2s3 = s2*s3;
   double s2c3 = s2*c3;
   float x = (float)( c1*c2s3 + s1*s2c3 );
   float y = (float)( s1*c2c3 - c1*s2s3 );
   float z = (float)( c1*s2c3 - s1*c2s3 );
   float w = (float)( c1*c2c3 + s1*s2s3 );
   set( x, y, z, w );
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::setFromShortestRotation( const Vector& v1, const Vector& v2 )
{
   const FastFloat d = v1.dot( v2 ); 

   Vector axis;
   axis.setCross( v1, v2 );

   FastFloat qw;
   qw.setMul( v1.lengthSq(), v2.lengthSq() );
   qw.sqrt();
   qw.add( d );
   if ( qw < Float_1e_4 ) 
   { 
      // vectors are 180 degrees apart
      set( -v1[2], v1[1], v1[0], 0.0f );
   } 
   else
   {
      set( axis[0], axis[1], axis[2], qw.getFloat() );
   }

   // normalize the quaternion
   normalize();
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::removeAxisComponent( const Vector& axis )
{
   // rotate the specified axis
   Vector rotatedAxis;
   transform( axis, rotatedAxis );

   const FastFloat dot = axis.dot( rotatedAxis );
   FastFloat negAngleDiff, posAngleDiff;
   negAngleDiff.setSub( dot, Float_1 );
   posAngleDiff.setAdd( dot, Float_1 );

   if ( negAngleDiff > Float_Minus1e_3 )
   {
      // the axis is the main rotation axis, so there's nothing
      // left from the quaternion's rotation now
      setIdentity();
   }
   else if ( posAngleDiff < Float_1e_3 )
   {
      // the axis goes in the opposite direction
      Vector perpVector;
      VectorUtil::calculatePerpendicularVector( axis, perpVector );
      perpVector.normalize();
      
      perpVector[3] = 0.0f;
      set( perpVector.m_quad );
   }
   else
   {
      FastFloat rotationAngle; 
      rotationAngle.setAcos( dot );

      Vector rotationAxis;
      rotationAxis.setCross( axis, rotatedAxis );
      rotationAxis.normalize();
      setAxisAngle( rotationAxis, rotationAngle );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Quaternion::decompose( const Vector& decompositionAxis, Quaternion& outRemainingQuaternion, FastFloat& outAngle ) const
{
   ASSERT_MSG( decompositionAxis.isNormalized(), "The decomposition axis must be a unit-length vector." );
   
   Quaternion axisRot;
   {
      outRemainingQuaternion = *this;
      outRemainingQuaternion.removeAxisComponent( decompositionAxis );

      // axisRot = inv(rest) * q
      Quaternion inv_rest;
      inv_rest.setConjugated( outRemainingQuaternion );
      axisRot.setMul( inv_rest, *this );
   }

   axisRot.getAngle( outAngle );
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<( std::ostream& stream, const Quaternion& rhs )
{
   stream << "[" << rhs[0] << ", " << rhs[1] << ", " << rhs[2] << ", " << rhs[3] << "]";
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& serializer, const Quaternion& rhs )
{
   serializer << rhs[0];
   serializer << rhs[1];
   serializer << rhs[2];
   serializer << rhs[3];
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& serializer, Quaternion& rhs )
{
   serializer >> rhs[0];
   serializer >> rhs[1];
   serializer >> rhs[2];
   serializer >> rhs[3];
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////
