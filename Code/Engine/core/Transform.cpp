#include "core/Transform.h"
#include "core/Matrix.h"
#include "core/EulerAngles.h"
#include "core/Plane.h"


///////////////////////////////////////////////////////////////////////////////

Transform Transform::IDENTITY;

///////////////////////////////////////////////////////////////////////////////

Transform::Transform()
{
   m_translation.setZero();
   m_rotation.setIdentity();
}

///////////////////////////////////////////////////////////////////////////////

Transform::Transform( const Quaternion& rotation )
   : m_rotation( rotation )
{
   m_translation.setZero();
}

///////////////////////////////////////////////////////////////////////////////

Transform::Transform( const Vector& translation )
   : m_translation( translation )
{
   m_rotation.setIdentity();
}

///////////////////////////////////////////////////////////////////////////////

Transform::Transform( const Vector& translation, const Quaternion& rotation )
   : m_translation( translation )
   , m_rotation( rotation )
{
}

///////////////////////////////////////////////////////////////////////////////

void Transform::set( const Vector& rotationAxis, const FastFloat& rotationAngle, const Vector& translation )
{
   m_translation = translation;
   m_rotation.setAxisAngle( rotationAxis, rotationAngle );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::set( const Vector& rotationAxis, float rotationAngle, const Vector& translation )
{
   m_translation = translation;
   m_rotation.setAxisAngle( rotationAxis, rotationAngle );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::set( const Quaternion& rotation, const Vector& translation )
{
   m_translation = translation;
   m_rotation = rotation;
}

///////////////////////////////////////////////////////////////////////////////

Transform& Transform::operator=( const Transform& rhs )
{
   m_translation = rhs.m_translation;
   m_rotation = rhs.m_rotation;
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

bool Transform::operator==( const Transform& rhs ) const
{
   return m_translation == rhs.m_translation && m_rotation == rhs.m_rotation;
}

///////////////////////////////////////////////////////////////////////////////

bool Transform::operator!=( const Transform& rhs ) const
{
   return !( *this == rhs );
}

///////////////////////////////////////////////////////////////////////////////

Transform& Transform::setRotation( const EulerAngles& angles )
{
   m_rotation.setFromEulerAngles( angles );
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

void Transform::getRotation( EulerAngles& outAngles ) const
{
   outAngles.setFromQuaternion( m_rotation );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::toMatrix( Matrix& outMatrix ) const
{
   outMatrix.setRotation( m_rotation );
   outMatrix.setPosition<3>( m_translation );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::set( const Matrix& matrix )
{
   m_translation = matrix.position();
   matrix.getRotation( m_rotation );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::setMul( const Transform& a, const Transform& b )
{
   b.m_rotation.transform( a.m_translation, m_translation );
   m_translation.add( b.m_translation );

   m_rotation.setMul( a.m_rotation, b.m_rotation );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::mul( const Transform& a )
{
   Vector tmpTranslation;
   a.m_rotation.transform( m_translation, tmpTranslation );
   m_translation.setAdd( a.m_translation, tmpTranslation );

   m_rotation.mul( a.m_rotation );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::preMul( const Transform& a )
{
   Vector tmpTranslation;
   m_rotation.transform( a.m_translation, tmpTranslation );
   m_translation.add( tmpTranslation );

   Quaternion newRot;
   newRot.setMul( a.m_rotation, m_rotation );
   m_rotation = newRot;
}

///////////////////////////////////////////////////////////////////////////////

void Transform::setMulInverse( const Transform& a, const Transform& b )
{
   // inlined setInverse
   Transform conjB;
   conjB.m_rotation.setConjugated( b.m_rotation );
   conjB.m_rotation.transform( b.m_translation, conjB.m_translation );
   conjB.m_translation.neg();

   // inlined setMul
   conjB.m_rotation.transform( a.m_translation, m_translation );
   m_translation.add( conjB.m_translation );

   m_rotation.setMul( a.m_rotation, conjB.m_rotation );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::setInverse( const Transform& rhs )
{
   m_rotation.setConjugated( rhs.m_rotation );
   m_rotation.transform( rhs.m_translation, m_translation );
   m_translation.neg();
}

///////////////////////////////////////////////////////////////////////////////

void Transform::invert()
{
   m_rotation.conjugate();

   Vector invertedTranslation;
   m_rotation.transform( m_translation, invertedTranslation );
   m_translation = invertedTranslation;
   m_translation.neg();
}

///////////////////////////////////////////////////////////////////////////////

void Transform::setSlerp( const Transform& a, const Transform& b, const FastFloat& t )
{
   m_rotation.setSlerp( a.m_rotation, b.m_rotation, t );
   m_translation.setLerp( a.m_translation, b.m_translation, t );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::setNlerp( const Transform& a, const Transform& b, const FastFloat& t )
{
   m_rotation.setNlerp( a.m_rotation, b.m_rotation, t );
   m_translation.setLerp( a.m_translation, b.m_translation, t );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::transform( const Vector& inVec, Vector& outVec ) const
{
   m_rotation.transform( inVec, outVec );
   outVec.add( m_translation );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::transformNorm( const Vector& inNorm, Vector& outNorm ) const
{
   m_rotation.transform( inNorm, outNorm );
}

///////////////////////////////////////////////////////////////////////////////

void Transform::transform( const Plane& inPlane, Plane& outPlane ) const
{
   Vector planeNormal;
   inPlane.getNormal( planeNormal );

   Vector rotatedPlaneNormal;
   m_rotation.transform( planeNormal, rotatedPlaneNormal );

   Vector newDistVec;
   newDistVec[3] = rotatedPlaneNormal.dot( m_translation ).getFloat();
   rotatedPlaneNormal[3] = inPlane[3];
   
   Vector newPlaneEq;
   newPlaneEq.setSub( rotatedPlaneNormal, newDistVec );
   outPlane.set( newPlaneEq );
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<( std::ostream& stream, const Transform& rhs )
{
   stream << "Translation: " << rhs.m_translation << " Rotation: " << rhs.m_rotation;
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& serializer, const Transform& rhs )
{
   serializer << rhs.m_translation;
   serializer << rhs.m_rotation;
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& serializer, Transform& rhs )
{
   serializer >> rhs.m_translation;
   serializer >> rhs.m_rotation;
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////
