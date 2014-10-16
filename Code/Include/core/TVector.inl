#ifndef _TVECTOR_H
#error "This file can only be included from TVector.h"
#else

#include "core\OutStream.h"
#include "core\InStream.h"



///////////////////////////////////////////////////////////////////////////////

template< int VDim >
std::ostream& operator<<( std::ostream& stream, const TVector< VDim >& rhs )
{
   stream << "[";

   for ( int i = 0; i < VDim; ++i )
   {
      if ( i > 0 )
      {
         stream << ", ";
      }
      stream << rhs.v[i];
   }

   stream << "]";
   return stream;
}

//////////////////////////////////////////////////////////////////////////////

template< int VDim >
OutStream& operator<<( OutStream& serializer, const TVector< VDim >& rhs )
{
   for ( int i = 0; i < VDim; ++i )
   {
      serializer << rhs.v[i];
   }
   return serializer;
}

///////////////////////////////////////////////////////////////////////////////

template< int VDim >
InStream& operator>>( InStream& serializer, TVector< VDim >& rhs )
{
   for ( int i = 0; i < VDim; ++i )
   {
      serializer >> rhs.v[i];
   }
   return serializer;
}

//////////////////////////////////////////////////////////////////////////////

template< int Dim >
float& TVector< Dim >::operator[]( int idx )
{
   return v[idx];
}

///////////////////////////////////////////////////////////////////////////////

template< int Dim >
float TVector< Dim >::operator[]( int idx ) const
{
   return v[idx];
}

///////////////////////////////////////////////////////////////////////////////

template< int Dim >
void TVector< Dim >::setUniform( float val )
{
   for ( int i = 0; i < Dim; ++i )
   {
      v[i] = val;
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _TVECTOR_H
