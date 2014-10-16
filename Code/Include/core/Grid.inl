#ifndef _GRID_H
#error "This file can only be included in Grid.h"
#else

#include "core\Point.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
Grid< T >::Grid( uint width, uint height, const T& defaultVal, const T& borderVal )
   : m_width( width )
   , m_height( height )
   , m_grid( new Array< T >( width * height ) )
   , m_deleteStorage( true )
   , m_borderVal( borderVal )
{
   m_grid->resize( width * height, defaultVal );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Grid< T >::Grid( uint width, uint height, typename Array< T >* externalStorage, const T& borderVal )
   : m_width( width )
   , m_height( height )
   , m_grid( externalStorage )
   , m_deleteStorage( false )
   , m_borderVal( borderVal )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Grid< T >::~Grid()
{
   if ( m_deleteStorage )
   {
      delete m_grid;
      m_grid = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Grid< T >::fill( const T& val )
{
   m_grid->broadcastValue( val );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Grid< T >::copyFrom( const typename Grid< T >& rhs )
{
   ASSERT_MSG( m_width == rhs.width() && m_height == rhs.height(), "Grid::copyFrom operation requires both grids to be of the same size" );

   memcpy( *m_grid, *rhs.m_grid, sizeof( T ) * m_width * m_height );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Grid< T >::setScaledSection( const typename Grid< T >& rhs, const Point& minPt, const Point& maxPt )
{   
   const float areaWidth = ( float ) ( maxPt.x - minPt.x );
   const float areaHeight = ( float ) ( maxPt.y - minPt.y );
   const float horizScale = areaWidth / ( float ) m_width;
   const float vertScale = areaHeight / ( float ) m_height;

   Point srcPt, destPt;
   uint destAddr = 0;
   for ( destPt.y = 0; destPt.y < (int)m_height; ++destPt.y )
   {
      for ( destPt.x = 0; destPt.x < ( int ) m_width; ++destPt.x, ++destAddr )
      {
         // get the source and target addr
         srcPt.x = ( int ) floor( ( float ) destPt.x * horizScale + minPt.x );
         srcPt.y = ( int ) floor( ( float ) destPt.y * vertScale  + minPt.y );

         (*m_grid)[destAddr] = rhs.getValue( srcPt );
      }
   }  
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& Grid< T >::getValue( const Point& cell ) const
{
   if ( cell.x < 0 || cell.x >= ( int ) m_width || cell.y < 0 || cell.y >= ( int ) m_height )
   {
      return m_borderVal;
   }

   const uint addr = cell.y * m_width + cell.x;
   return (*m_grid)[addr];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
byte Grid< T >::countNeighboursEqualTo( const Point& cell, const T& val ) const
{
   byte count = 0;
   count += ( byte ) ( getValue( cell + Point( -1, 0 ) ) == val );
   count += ( byte ) ( getValue( cell + Point(  1, 0 ) ) == val );
   count += ( byte ) ( getValue( cell + Point( 0, -1 ) ) == val );
   count += ( byte ) ( getValue( cell + Point( 0,  1 ) ) == val );
   count += ( byte ) ( getValue( cell ) == val );

   return count;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Grid< T >::toString( std::string& outStr ) const
{
   outStr = "";
   uint addr = 0;
   for ( uint y = 0; y < m_height; ++y )
   {
      for ( uint x = 0; x < m_width; ++x, ++addr )
      {
         ::toString( ( *m_grid )[addr], outStr );
      }
      outStr += "\n";
   }
}

///////////////////////////////////////////////////////////////////////////////


#endif // _GRID_H
