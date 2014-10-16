#ifndef _OUT_STREAM_H
#error "This file can only be included from OutStream.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< typename T >
OutStream& OutStream::operator<<( const T& val )
{
   save( (void*)&val, sizeof( T ) );
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
OutStream& OutStream::operator<<( const std::vector< T >& val )
{
   // save the array size
   uint size = val.size();
   save( (void*)&size, sizeof( uint ) );

   // save the elements
   for ( uint i = 0; i < size; ++i )
   {
      save( (void*)&val[i], sizeof( T ) );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
OutStream& OutStream::operator<<( const Array< T >& val )
{
   // save the array size
   uint size = val.size();
   save( (void*)&size, sizeof( uint ) );

   // save the elements
   for ( uint i = 0; i < size; ++i )
   {
      save( (void*)&val[i], sizeof( T ) );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////


#endif // _OUT_STREAM_H
