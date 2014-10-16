#ifndef _IN_STREAM_H
#error "This file can only be included from InStream.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< typename T >
InStream& InStream::operator>>( T& val )
{
   load( (void*)&val, sizeof( T ) );
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
InStream& InStream::operator>>( std::vector< T >& val )
{
   // load the array size
   uint size;
   load( (void*)&size, sizeof( uint ) );

   // resize the array
   val.resize( size );

   // load the elements
   for ( uint i = 0; i < size; ++i )
   {
      load( (void*)&val[i], sizeof( T ) );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
InStream& InStream::operator>>( Array< T >& val )
{
   // load the array size
   uint size;
   load( (void*)&size, sizeof( uint ) );

   // resize the array
   val.resize( size );

   // load the elements
   for ( uint i = 0; i < size; ++i )
   {
      load( (void*)&val[i], sizeof( T ) );
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _IN_STREAM_H
