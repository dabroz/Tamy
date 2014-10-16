/// @file   core\Grid.h
/// @brief  a simple grid data structure
#ifndef _GRID_H
#define _GRID_H

#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\Array.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

struct Point;

///////////////////////////////////////////////////////////////////////////////

/**
 * A simple grid data structure.
 */
template< typename T >
class Grid
{
   DECLARE_ALLOCATOR( Grid, AM_DEFAULT );

private:
   uint           m_width;
   uint           m_height;

   Array< T >*    m_grid;
   bool           m_deleteStorage;

   T              m_borderVal;

public:
   /**
    * Constructor.
    *
    * @param width
    * @param height
    * @param defaultVal
    * @param borderVal
    */
   Grid( uint width, uint height, const T& defaultVal = 0, const T& borderVal = 0 );

   /**
    * External storage constructor.
    *
    * @param width
    * @param height
    * @param externalStorage
    * @param borderVal
    */
   Grid( uint width, uint height, typename Array< T >* externalStorage, const T& borderVal = 0 );
   ~Grid();

   /**
    * Fills the entire grid with a value.
    *
    * @param val
    */
   void fill( const T& val );

   /**
    * Copies the contents of the other grid to this one.
    *
    * CAUTION: the grids must be of the same size
    *
    * @param rhs
    */
   void copyFrom( const typename Grid< T >& rhs );

   /**
    * Fills the grid with the scaled contents of the selected section of the other grid.
    *
    * @param rhs        the other grid
    * @param minPt
    * @param maxPt
    */
   void setScaledSection( const typename Grid< T >& rhs, const Point& minPt, const Point& maxPt );

   /**
    * Gives access to the value located in the grid cell with the specified address.
    * CAUTION: This operation does not check the boundary conditions - the user needs to take care of that himself.
    *
    * @param address
    */
   inline T& operator[]( uint address ) {
      return (*m_grid)[address];
   }

   /**
    * Returns the value located in the grid cell with the specified address.
    * CAUTION: This operation does not check the boundary conditions - the user needs to take care of that himself.
    *
    * @param address
    */
   inline const T& operator[]( uint address ) const {
      return (*m_grid)[address];
   }

   /**
    * Gives access to the value located at the specific cell of the grid.
    * CAUTION: This operation does not check the boundary conditions - the user needs to take care of that himself.
    *
    * @param address
    */
   inline T& operator()( uint x, uint y ) {
      return ( *m_grid )[y * m_width + x];
   }

   /**
    * Gives access to the value located at the specific cell of the grid.
    * CAUTION: This operation does not check the boundary conditions - the user needs to take care of that himself.
    *
    * @param address
    */
   inline const T& operator()( uint x, uint y ) const {
      return ( *m_grid )[y * m_width + x];
   }

   /**
    * Returns the value located under the specified coordinates.
    * The operation runs a boundary condition check and returns the default value
    * if the boundaries have been violated.
    */
   const T& getValue( const Point& cell ) const;

   /**
    * Returns a number of neighbors of the cell that have the value equal to the specified one.
    * The count will include the specified cell.
    *
    * @param pt
    * @param val
    */
   byte countNeighboursEqualTo( const Point& cell, const T& val ) const;

   /**
    * Calculates the address of the specified cell.
    *
    * @param cell
    */
   inline uint calcAddr( const Point& cell ) const {
      return cell.y * m_width + cell.x;
   }

   /**
    * Returns the width of the grid.
    */
   inline uint width() const {
      return m_width;
   }

   /**
   * Returns the height of the grid.
   */
   inline uint height() const {
      return m_height;
   }

   /**
    * Renders the grid to a textual form, rendering every cell's contents using the specified
    * value dictionary.
    *
    * @param outStr
    */
   void toString( std::string& outStr ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core\Grid.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRID_H
