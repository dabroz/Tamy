/// @file   core\GridUtils.h
/// @brief  a set of functions that operate on grids
#ifndef _GRID_UTILS_H
#define _GRID_UTILS_H

#include "core\List.h"
#include "core\Grid.h"

///////////////////////////////////////////////////////////////////////////////

struct Point;

///////////////////////////////////////////////////////////////////////////////

typedef uint( *GridTraversalCostFunction )( const Point&, const Point& );

#define GRID_CELLCOST_BLOCKED 0xFFFF

///////////////////////////////////////////////////////////////////////////////

template< typename T >
struct GridSearchInfo
{
   typedef uint( *CellCostFunction )( const Grid< T >&, const Point& );

   // input
   Point                               m_start;
   Point                               m_end;
   GridTraversalCostFunction           m_traversalCostFunction;
   typename CellCostFunction           m_cellCostFunction;

   // output
   List< Point >                       m_pathPoints;

   // some debug statistics
   uint                                m_numIterations;        // how many iterations did it take to complete the search

   GridSearchInfo()
      : m_numIterations( 0 )
   {}
};

///////////////////////////////////////////////////////////////////////////////

template< typename T >
class GridUtils
{
public:
   /**
    * Runs an A* algorithm on a grid.
    *
    * @param grid
    * @param searchInfo
    * @return  was the path found?
    */
   static bool aStar( typename const Grid< T >& grid, typename GridSearchInfo< T >& searchInfo );

private:
   /**
    * A helper method used by the aStar algorithm.
    */
   static void popCheapest( typename const Grid< T >& grid, List< Point >& openList, const Array< uint >& openCostsMap, Point& outPos );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\GridUtils.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRID_UTILS_H
