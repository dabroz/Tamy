/// @file   core-Point.h
/// @brief  a simple point representation
#ifndef _POINT_H
#define _POINT_H

#include <iostream>
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

struct Vector;
class InStream;
class OutStream;

///////////////////////////////////////////////////////////////////////////////

/**
 * A simple point representation.
 */
struct Point
{
   DECLARE_ALLOCATOR( Point, AM_DEFAULT );

   union
   {
      struct
      {
         int x;
         int y;
      };

      int v[2];
   };


   /**
    * Default constructor.
    */
   Point();

   /**
    * Parametric constructor.
    *
    * @param _x
    * @param _y
    */
   Point( int _x, int _y );

   /**
    * Cast constructor.
    *
    * @param vec
    */
   Point( const Vector& vec );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   Point( const Point& rhs );

   /**
    * Creates a direction vector out of the specified point.
    *
    * @param pt
    */
   void setAsDirection( const Point& pt );

   /**
    * Assigns the point new coordinates.
    *
    * @param _x
    * @param _y
    */
   inline void set( int _x, int _y );

   /**
    * Converts the point to a vector.
    *
    * @param outVec
    */
   inline void toVector( Vector& outVec ) const;

   /**
    * Calculates a squared distance to another point.
    *
    * @param rhs
    */
   inline float distanceSqTo( const Point& rhs ) const;

   /**
    * Calculates a distance to another point.
    *
    * @param rhs
    */
   inline float distanceTo( const Point& rhs ) const;

   /**
    * Returns the squared distance from the origin to this point.
    */
   inline float lengthSq() const;

   /**
    * Returns the distance from the origin to this point.
    */
   inline float length() const;

   // -------------------------------------------------------------------------
   // Comparison operators
   // -------------------------------------------------------------------------
   inline bool operator==( const Point& rhs ) const;
   inline bool operator!=( const Point& rhs ) const;

   // -------------------------------------------------------------------------
   // Arithmetic operators
   // -------------------------------------------------------------------------
   inline Point operator-( const Point& rhs ) const;
   inline Point operator+( const Point& rhs ) const;
   inline Point operator*( int val ) const;
   inline void operator+=( const Point& rhs );
   inline void operator-=( const Point& rhs );
   inline void operator*=( int val );
   inline void operator/=( int val );

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend std::ostream& operator<<( std::ostream& stream, const Point& rhs );
   friend OutStream& operator<<( OutStream& serializer, const Point& rhs );
   friend InStream& operator>>( InStream& serializer, Point& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\Point.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _POINT_H
