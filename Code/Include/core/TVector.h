/// @file   core\TVector.h
/// @brief  raw vector composed of T elements
#ifndef _TVECTOR_H
#define _TVECTOR_H

#include <iostream>


///////////////////////////////////////////////////////////////////////////////

class OutStream;
class InStream;

///////////////////////////////////////////////////////////////////////////////

/**
 * A vector that can have an arbitrary, but fixed number of coordinates.
 *
 * It's not a mathematical vector though - it's just a data structure.
 * That's why it should not have any mathematical operations defined!!!
 * Instead, create a class that contains it and contains relevant math functions
 */
template< int Dim >
struct TVector
{
private:
   enum { ARR_SIZE = Dim };

public:

   float v[ARR_SIZE];

   // -------------------------------------------------------------------------
   // Operators
   // -------------------------------------------------------------------------
   float& operator[]( int idx );
   float operator[]( int idx ) const;

   // -------------------------------------------------------------------------
   // Operations
   // -------------------------------------------------------------------------

   /**
    * Sets a single value for all coordinates.
    */
   void setUniform( float val );

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   template< int VDim > friend std::ostream& operator<<( std::ostream& stream, const TVector< VDim >& rhs );
   template< int VDim > friend OutStream& operator<<( OutStream& serializer, const TVector< VDim >& rhs );
   template< int VDim > friend InStream& operator>>( InStream& serializer, TVector< VDim >& rhs );
};

///////////////////////////////////////////////////////////////////////////////

#include "core/TVector.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _TVECTOR_H
