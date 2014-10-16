#ifndef _GRID_UTILS_H
#error "This file can only be included in GridUtils.h"
#else

#include "core\Point.h"
#include "core\Assert.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool GridUtils< T >::aStar( typename const Grid< T >& grid, typename GridSearchInfo< T >& searchInfo )
{
   // reset statistics before the search starts
   searchInfo.m_numIterations = 0;

   // create auxiliary data structures
   const uint nodesCount = grid.width() * grid.height();

   Array< uint > openCostsMap( nodesCount );     // this array holds the costs of the path that is being built
   openCostsMap.resize( nodesCount, 0 );

   Array< Point > parentNode( nodesCount );    // this array stores the indices of nodes that lead us to the adjacent node
   parentNode.resize( nodesCount, Point( -1, -1 ) );

   List< Point > closedList;
   List< Point > openList;
   const Point dPos[] = { Point( -1, 0 ), Point( 0, 1 ), Point( 1, 0 ), Point( 0, -1 ) };

   Point candidatePos;
   bool wasPathFound = false;

   // start the search
   openList.pushBack( searchInfo.m_start );
   while ( !openList.empty() )
   {
      // gather some statistics
      ++searchInfo.m_numIterations;

      // find the cheapest node in the open nodes list and 'promote' it
      // to the closed list
      popCheapest( grid, openList, openCostsMap, candidatePos );
      closedList.pushBack( candidatePos );

      if ( candidatePos == searchInfo.m_end )
      {
         // reached it - finish the search
         wasPathFound = true;
         break;
      }

      // analyze the cheapest node's neighbors
      for ( uint dir = 0; dir < 4; ++dir )
      {
         Point adjacentPos = candidatePos + dPos[dir];
         const int adjacentCellCost = searchInfo.m_cellCostFunction( grid, adjacentPos );
         if ( adjacentCellCost >= GRID_CELLCOST_BLOCKED )
         {
            // can't go that way
            continue;
         }

         // if the node's already in the closed list - discard it
         List< Point >::iterator isOnClosedList = ListUtils::find( closedList, adjacentPos );
         if ( !isOnClosedList.isEnd() )
         {
            continue;
         }

         const uint adjacentPosAddr = grid.calcAddr( adjacentPos );

         // calculate the cost of this transition ( directing our steps towards the end point )
         int distanceCost = searchInfo.m_traversalCostFunction( adjacentPos, searchInfo.m_end );
         int newCost = adjacentCellCost + distanceCost;

         // if the node's already in the open list, relax it, 
         // and if it's not - add it
         List< Point >::iterator openListIt = ListUtils::find( openList, adjacentPos );
         if ( !openListIt.isEnd() )
         {
            const int existingCost = openCostsMap[adjacentPosAddr];
            if ( existingCost > newCost )
            {
               openCostsMap[adjacentPosAddr] = newCost;
               parentNode[adjacentPosAddr] = candidatePos;
            }
         }
         else
         {
            openList.pushBack( adjacentPos );
            openCostsMap[adjacentPosAddr] = newCost;
            parentNode[adjacentPosAddr] = candidatePos;
         }
      }
   }

   // now that we've reached the end, plot the path
   if ( wasPathFound == true )
   {
      for ( Point currPos = searchInfo.m_end; currPos != searchInfo.m_start; )
      {
         searchInfo.m_pathPoints.pushBack( currPos );

         const int currPosAddr = grid.calcAddr( currPos );
         currPos = parentNode[currPosAddr];

         ASSERT( currPos != Point( -1, -1 ) );
      }
      searchInfo.m_pathPoints.pushBack( searchInfo.m_start );
   }

   return wasPathFound;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void GridUtils< T >::popCheapest( typename const Grid< T >& grid, List< Point >& openList, const Array< uint >& openCostsMap, Point& outPos )
{
   uint bestCost = GRID_CELLCOST_BLOCKED;

   for ( List< Point >::iterator it = openList.begin(); !it.isEnd(); ++it )
   {
      const Point& pos = *it;
      const uint cellCost = openCostsMap[grid.calcAddr( pos ) ];
      if ( cellCost < bestCost )
      {
         bestCost = cellCost;
         outPos = pos;
      }
   }

   // remove the selected node
   List< Point >::iterator it = ListUtils::find( openList, outPos );
   it.markForRemoval();
}

///////////////////////////////////////////////////////////////////////////////


#endif // _GRID_UTILS_H
