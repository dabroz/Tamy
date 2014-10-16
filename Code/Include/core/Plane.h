/// @file   core/PLane.h
/// @brief  mathematical plane representation
#ifndef _PLANE_H
#define _PLANE_H

#include <iostream>
#include "core\MathDataStorage.h"
#include "core\FastFloat.h"
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class OutStream;
class InStream;
struct Vector;

///////////////////////////////////////////////////////////////////////////////

/**
 * Mathematical plane representation.
 */
ALIGN_16 struct Plane
{
   DECLARE_ALLOCATOR( Plane, AM_ALIGNED_16 );

   QuadStorage     m_quad;

   /**
    * Creates a null plane.
    */
   inline void setZero();

   /**
    * Creates a plane by adapting vector components as plane equation components.
    *
    * @param vec
    */
   inline void set( const Vector& vec );

   /**
    * Creates a plane from a normal and a distance to origin
    *
    * @param normal    plane normal
    * @param distance  plane normal
    */
   inline void set( const Vector& normal, const FastFloat& distance );

   /**
    * Creates a plane from four float components.
    *
    * @param a, b, c, d    plane parameters
    */
   inline void set( const FastFloat& a, const FastFloat& b, const FastFloat& c, const FastFloat& d );

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   inline void operator=( const Plane& rhs );
   inline bool operator==( const Plane& rhs ) const;
   inline bool operator!=( const Plane& rhs ) const;

   /**
    * This operator gives access to individual plane components.
    *
    * @param idx        requested component idx
    */
   inline float& operator[]( int idx );
   inline float operator[]( int idx ) const;

   /**
    * Returns the specified plane component.
    *
    * @param idx
    */
   inline const FastFloat getComponent( uint idx ) const;

   // -------------------------------------------------------------------------
   // Operations
   // -------------------------------------------------------------------------

   /**
    * Determines a plane based on a point that lies on the plane and the normal of the plane.
    *
    * @param point
    * @param normal
    */
   void setFromPointNormal( const Vector& point, const Vector& normal );

   /**
    * Determines a plane based on three specified coplanar points.
    *
    * @param p1
    * @param p2
    * @param p3
    */
   void setFromPoints( const Vector& p1, const Vector& p2, const Vector& p3 );

   /**
    * Normalizes the plane
    */
   inline void normalize();

   /**
    * Calculates a dot product treating the specified vector as a coordinate in space.
    * Perfect for calculating a distance from the plane to a point in space.
    *
    * @param coord
    */
   inline const FastFloat dotCoord( const Vector& coord ) const;

   /**
    * Calculates a dot product treating the specified vector as a normal vector ( unit length vector ).
    *
    * @param normal
    */
   inline const FastFloat dotNormal( const Vector& normal ) const;

   /**
    * Returns the normal of the plane.
    */
   inline void getNormal( Vector& outPlaneNormal ) const;


   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend std::ostream& operator<<( std::ostream& stream, const Plane& rhs );
   friend OutStream& operator<<( OutStream& serializer, const Plane& rhs );
   friend InStream& operator>>( InStream& serializer, Plane& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#ifdef _USE_SIMD
   #include "core/PlaneSimd.inl"
#else
   #include "core/PlaneFpu.inl"
#endif

///////////////////////////////////////////////////////////////////////////////

#endif // _PLANE_H
