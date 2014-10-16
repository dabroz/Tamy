/// @file   core/Transform.h
/// @brief  mathematical transform definition
#ifndef _TRANSFORM_H
#define _TRANSFORM_H

#include <iostream>
#include "core/types.h"
#include "core/Vector.h"
#include "core/Quaternion.h"


///////////////////////////////////////////////////////////////////////////////

struct Vector;
struct Quaternion;
struct EulerAngles;
struct Plane;
struct Matrix;
class OutStream;
class InStream;

///////////////////////////////////////////////////////////////////////////////

/**
 * Mathematical transform definition. This is an affine transform that 
 * rotates and translates ( in that exact order ) while preserving the scale
 * of the transformed objects.
 */
ALIGN_16 struct Transform
{
   DECLARE_ALLOCATOR( Transform, AM_ALIGNED_16 );

   Vector            m_translation;
   Quaternion        m_rotation;

   static Transform IDENTITY;

   /**
    * Default constructor.
    */
   Transform();

   /**
    * Constructor that creates a rotation transform.
    * We want it to be called explicitly and not as a conversion constructor.
    *
    * @param translation
    * @param rotation
    */
   explicit Transform( const Quaternion& rotation );


   /**
    * Constructor that creates a translation transform.
    * We want it to be called explicitly and not as a conversion constructor.
    *
    * @param translation
    * @param rotation
    */
   explicit Transform( const Vector& translation );


   /**
    * Constructor.
    *
    * @param translation
    * @param rotation
    */
   Transform( const Vector& translation, const Quaternion& rotation );

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   Transform& operator=( const Transform& rhs );
   bool operator==( const Transform& rhs ) const;
   bool operator!=( const Transform& rhs ) const;

   // -------------------------------------------------------------------------
   // Operations
   // -------------------------------------------------------------------------

   /**
    * Creates a rotation matrix with rotation corresponding to the specified Euler angle value
    *
    * @param angles
    */
   Transform& setRotation( const EulerAngles& angles );

   /**
    * Returns the rotation this matrix describes in the form of Euler angles.
    *
    * @param outAngles
    */
   void getRotation( EulerAngles& outAngles ) const;

   /**
    * Converts the transform to a regular 4x4 matrix
    *
    * @param outMatrix
    */
   void toMatrix( Matrix& outMatrix ) const;

   /**
    * Converts a 4x4 matrix to a transform
    *
    * @param matrix
    */
   void set( const Matrix& matrix );

   /**
    * this = a * b
    *
    * @param a
    * @param b
    */
   void setMul( const Transform& a, const Transform& b );

   /**
    * this = this * a
    *
    * CAUTION - it allocates temporary data on stack, thus it's slower than setMul.
    *
    * @param a
    */
   void mul( const Transform& a );

   /**
    * this = a * this
    *
    * CAUTION - it allocates temporary data on stack, thus it's slower than setMul.
    *
    * @param a
    */
   void preMul( const Transform& a );

   /**
    * this = a * b^-1
    *
    * @param a
    * @param b
    */
   void setMulInverse( const Transform& a, const Transform& b );

   /**
    * Sets an inverted transform.
    *
    * @param rhs
    */
   void setInverse( const Transform& rhs );

   /**
    * Inverts the transform.
    */
   void invert();

   /**
    * this = a*t + b*(1-t)
    *
    * Rotation will be spherically interpolated.
    *
    * @param a
    * @param b
    * @param t
    */
   void setSlerp( const Transform& a, const Transform& b, const FastFloat& t ); 

   /**
    * this = a*t + b*(1-t)
    *
    * Rotation will be linearly interpolated.
    *
    * @param a
    * @param b
    * @param t
    */
   void setNlerp( const Transform& a, const Transform& b, const FastFloat& t );

   /**
    * Transforms the specified vector.
    *
    * @param inVec     vector to be transformed.
    * @param outVec  transformed vector
    */
   void transform( const Vector& inVec, Vector& outVec ) const;

   /**
    * Transforms the specified normal vector
    *
    * @param inNorm        normal vector to be transformed.
    * @param outNorm    transformed normal
    */
   void transformNorm( const Vector& inNorm, Vector& outNorm ) const;

   /**
    * Transforms the specified plane.
    *
    * @param inPlane     plane to be transformed.
    * @param outPlane  transformed plane
    */
   void transform( const Plane& inPlane, Plane& outPlane ) const;

   /**
    * This method writes the contents of the orientation to the specified 
    * output stream.
    *
    * @param stream     output stream
    * @param rhs        orientation description of which we want to output
    */
   friend std::ostream& operator<<( std::ostream& stream, const Transform& rhs );

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& serializer, const Transform& rhs );
   friend InStream& operator>>( InStream& serializer, Transform& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#endif // _TRANSFORM_H
