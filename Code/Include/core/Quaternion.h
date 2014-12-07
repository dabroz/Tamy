/// @file   core\Quaternion.h
/// @brief  quaternion representation
#ifndef _QUATERNION_H
#define _QUATERNION_H

#include <iostream>
#include "core\MathDataStorage.h"
#include "core\FastFloat.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

struct EulerAngles;
class OutStream;
class InStream;
struct Vector;

///////////////////////////////////////////////////////////////////////////////

/**
 * Quaternion representation.
 */
ALIGN_16 struct Quaternion
{
   DECLARE_ALLOCATOR( Quaternion, AM_ALIGNED_16 );

   static Quaternion IDENTITY;

   QuadStorage    m_quad;

   /**
    * Default constructor.
    */
   inline Quaternion();

   /**
    * Constructor.
    *
    * @param x, y, z, w       quaternion coordinates
    */
   inline Quaternion( float x, float y, float z, float w );

   /**
    * Constructor.
    *
    * @param quad
    */
   inline Quaternion( const QuadStorage& quad );

   /**
    * Sets an identity quaternion.
    */
   inline void setIdentity();

   /**
    * Creates a quaternion from a single quad.
    *
    * @param quad
    */
   inline void set( const QuadStorage& quad );

   /**
    * Creates a quaternion from four float coordinates.
    *
    * @param x, y, z, w       quaternion coordinates
    */
   inline void set( float x, float y, float z, float w );

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   inline void operator=( const Quaternion& rhs );
   inline bool operator==( const Quaternion& rhs ) const;
   inline bool operator!=( const Quaternion& rhs ) const;

   /**
    * This operator gives access to individual plane components.
    *
    * @param idx        requested component idx
    */
   inline float& operator[]( int idx );
   inline float operator[]( int idx ) const;

   // -------------------------------------------------------------------------
   // Setters
   // -------------------------------------------------------------------------

   /**
    * this = q1 + q2
    */
   inline void setAdd( const Quaternion& q1, const Quaternion& q2 );

   /**
    * this = q1 - q2
    */
   inline void setSub( const Quaternion& q1, const Quaternion& q2 );

   /**
    * this = q1 * q2
    */
   inline void setMul( const Quaternion& q1, const Quaternion& q2 );

   /**
    * this = this + q2
    */
   inline void add( const Quaternion& rhs );

   /**
    * this = this - q2
    */
   inline void sub( const Quaternion& rhs );

   /**
    * this = this * q2.
    */
   inline void mul( const Quaternion& rhs );

   /**
    * this = -this;
    */
   inline void neg();

   /**
    * Sets the inverse of the specified quaternion, which accounts for the same rotation, only in the opposite
    * direction.
    */
   inline void setConjugated( const Quaternion& rhs );

   /**
   * Inverts the specified quaternion, which accounts for the same rotation, only in the opposite
   * direction.
    */
   inline void conjugate();

   /**
    * Converts EulerAngles into a quaternion
    *
    * @param angles           Euler angles
    */
   void setFromEulerAngles( const EulerAngles& angles );

   /**
    * Creates a quaternion from an axis and an angle.
    * The axis vector NEEDS TO BE NORMALIZED
    *
    * @param axis
    * @param angle      expressed in RADIANS
    */
   inline void setAxisAngle( const Vector& axis, const FastFloat& angle );

   /**
   * Creates a quaternion from an axis and an angle.
   * The axis vector NEEDS TO BE NORMALIZED
   *
   * @param axis
   * @param angle      expressed in RADIANS
   */
   void setAxisAngle( const Vector& axis, float angle );

   /**
    * Creates a quaternion that describes the shortest rotation that would
    * transform one vector into another vector. The vectors NEED TO BE NORMALIZED.
    *
    * @param v1         start vector
    * @param v2         end vector
    */
   void setFromShortestRotation( const Vector& v1, const Vector& v2 );

   /**
    * Creates a new quaternion that is a result of a spherical interpolation
    * between two specified quaternions.
    *
    * @param a             start quaternion
    * @param b             end quaternion
    * @param t             interpolation distance
    */
   inline void setSlerp( const Quaternion& a, const Quaternion& b, const FastFloat& t );

   /**
    * Creates a new quaternion that is a result of a linear interpolation
    * between two specified quaternions.
    *
    * @param a             start quaternion
    * @param b             end quaternion
    * @param t             interpolation distance
    */
   inline void setNlerp( const Quaternion& a, const Quaternion& b, const FastFloat& t );

   /**
    * this = a + v * t
    *
    * @param a
    * @param v
    * @param t
    */
   inline void setAddScaledVector( const Quaternion& a, const Vector& v, const FastFloat& t );

   /**
    * this = this + v * t
    *
    * @param v
    * @param t
    */
   inline void addScaledVector( const Vector& v, const FastFloat& t );

   /**
    * Normalizes the quaternion.
    */
   inline void normalize();

   /**
    * Returns the axis this quaternion rotates about. 
    * 
    * CAUTION: This method doesn't work for identity quaternion,
    * because in that case there's no axis and angle defined to begin with.
    *
    * @param outAxis
    */
   inline void getAxis( Vector& outAxis ) const;

   /**
    * Returns the angle this quaternion rotates about ( in RADIANS ).
    *
    * CAUTION: This method doesn't work for identity quaternion,
    * because in that case there's no axis and angle defined to begin with.
    */
   inline void getAngle( FastFloat& outAngle ) const;

   /**
    * Removes the component that rotates this quaternion about the specified axis.
    */
   void removeAxisComponent( const Vector& axis );

   /**
    * Decomposes a quaternion about the specified axis, returning the angle that it would
    * rotate about and the remaining quaternion.
    *
    * WARNING: The specified axis MUST BE A UNIT-LENGTH VECTOR.
    *
    * @param decompositionAxis
    * @param outRemainingQuaternion
    * @return decomposed rotation angle ( in RADIANS )
    */
   void decompose( const Vector& decompositionAxis, Quaternion& outRemainingQuaternion, FastFloat& outAngle ) const;

   /**
    * Rotates a vector.
    *
    * CAUTION: don't specify the same vector twice here, as the results will be unpredictable.
    *
    * @param inVec      vector to transform
    * @param outVec     transformed vector
    */
   inline void transform( const Vector& inVec, Vector& outVec ) const;

   /**
    * This method writes the description of the quaternion to the specified 
    * output stream.
    *
    * @param stream     output stream
    * @param rhs        vector description of which we want to output
    */
   friend std::ostream& operator<<(std::ostream& stream, const Quaternion& rhs);

   // -------------------------------------------------------------------------
   // Storing/loading
   // -------------------------------------------------------------------------
   /**
    * Stores the contents of this vector in the specified array of 4 floats. The array needs to be address-aligned.
    *
    * @param arrFloats
    */
   inline void store( float* arrFloats ) const;

   /**
    * Loads the contents of the specified array of 4 floats into this vector. The array needs to be address-aligned.
    *
    * @param arrFloats
    */
   inline void load( const float* arrFloats );

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& serializer, const Quaternion& rhs );
   friend InStream& operator>>( InStream& serializer, Quaternion& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#ifdef _USE_SIMD
   #include "core/QuaternionSimd.inl"
#else
   #include "core/QuaternionFpu.inl"
#endif

///////////////////////////////////////////////////////////////////////////////

#endif // _QUATERNION_H
