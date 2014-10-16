/// @file   core\EulerAngles.h
/// @brief  Euler angles representation
#ifndef _EULER_ANGLES_H
#define _EULER_ANGLES_H

#include "core\MemoryRouter.h"
#include "core\MathDataStorage.h"
#include "core\FastFloat.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////

struct Vector;
struct Quaternion;
class OutStream;
class InStream;

///////////////////////////////////////////////////////////////////////////////

/**
 * Mathematical orientation - expressed by three angles to the respective axis
 * of the Cartesian system. The angular value is expressed in radians.
 */
ALIGN_16 struct EulerAngles
{
   DECLARE_ALLOCATOR( EulerAngles, AM_ALIGNED_16 );

   enum FieldsLayout
   {
      Yaw,
      Pitch,
      Roll,
      NotUsed
   };

   // data layout in the quad: { yaw, pitch, roll, not used }
   QuadStorage     m_quad;

   /**
    * Creates a null plane.
    */
   inline void setZero();

   /**
    * Creates an Euler angle from three float components.
    *
    * @param yaw       rotation around OY axis ( degrees )
    * @param pitch     rotation around OX axis ( degrees )
    * @param roll      rotation around OZ axis ( degrees )
    */
   inline void set( float yaw, float pitch, float roll );

   /**
    * Creates an Euler angle from three fast float components.
    *
    * @param yaw       rotation around OY axis ( degrees )
    * @param pitch     rotation around OX axis ( degrees )
    * @param roll      rotation around OZ axis ( degrees )
    */
   inline void set( const FastFloat& yaw, const FastFloat& pitch, const FastFloat& roll );

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   inline bool operator==( const EulerAngles& rhs ) const;
   inline bool operator!=( const EulerAngles& rhs ) const;
   inline void operator=( const EulerAngles& rhs );   

   /**
    * These operators gives access to individual angle components.
    *
    * @param idx        requested component idx
    */
   inline float& operator[]( int idx );
   inline float operator[]( int idx ) const;

   /**
    * These methods gives access to individual angle components.
    */
   inline float getYaw() const;
   inline float getPitch() const;
   inline float getRoll() const;

   // -------------------------------------------------------------------------
   // Operations
   // -------------------------------------------------------------------------

   /**
    * this = a + b
    */
   inline void setAdd( const EulerAngles& a, const EulerAngles& b );

   /**
    * this = a - b
    */
   inline void setSub( const EulerAngles& a, const EulerAngles& b );

   /**
    * this += a
    */
   inline void add( const EulerAngles& a );

   /**
    * this -= a
    */
   inline void sub( const EulerAngles& a );

   /**
    * Calculates the shortest rotation angle that transforms vec1 into vec2.
    *
    * @param vec1
    * @param vec2
    */
   void setFromShortestRotation( const Vector& vec1, const Vector& vec2 );

   /**
    * Constructor that converts a quaternion to Euler angles.
    *
    * CAUTION: This method is HIGHLY unstable in terms of produced results.
    * and as a matter of fact it should not be used for any computations
    * that require stability.
    *
    * As EulerAngles will mostly be used in the editor, to display 
    * the angles in a human readable form, and Quaternions will be used anywhere else,
    * I suggest that all applicable editor data be stored as EulerAngles and then
    * converted to Quaternion, as that method is highly stable in terms of produced results.
    *
    * @param quat
    */
   void setFromQuaternion( const Quaternion& quat );

   /**
    * Returns a normalized version of the orientation.
    * Angles of the normalized orientation are in range[0, 360)
    *
    * @param outNormalized
    */
   inline void normalized( EulerAngles& outNormalized ) const;

   /**
    * Normalizes this orientation.
    * Angles of the normalized orientation are in range[0, 360)
    */
   inline void normalize();

   /**
    * This method writes the contents of the orientation to the specified 
    * output stream.
    *
    * @param stream     output stream
    * @param rhs        orientation description of which we want to output
    */
   friend std::ostream& operator<<(std::ostream& stream, const EulerAngles& rhs);

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& serializer, const EulerAngles& rhs );
   friend InStream& operator>>( InStream& serializer, EulerAngles& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#ifdef _USE_SIMD
   #include "core/EulerAnglesSimd.inl"
#else
   #include "core/EulerAnglesFpu.inl"
#endif

///////////////////////////////////////////////////////////////////////////////

#endif // _EULER_ANGLES_H
