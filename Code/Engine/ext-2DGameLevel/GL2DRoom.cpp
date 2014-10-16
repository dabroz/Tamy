#include "ext-2DGameLevel\GL2DRoom.h"
#include "ext-2DGameLevel\GL2DLevelVoxels.h"
#include "ext-2DGameLevel\GL2DProceduralLevel.h"

// room parts
#include "ext-2DGameLevel\GL2DBounds.h"
#include "ext-2DGameLevel\GL2DRoomArea.h"

// voxels dictionary
#include "ext-2DGameLevel\GL2DVoxelPrefabsMap.h"
#include "ext-2DGameLevel\GL2DRoomTheme.h"
#include "ext-2DGameLevel\GL2DVoxelizedItem.h"

// utils
#include "core-MVC\Prefab.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntityUtils.h"
#include "core\ListUtils.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

GL2DRoom::GL2DRoom( const GL2DRoomTheme* theme, const Point& size, const Array< GL2DVoxelRotation >& arrExitDirections )
   : m_theme( theme )
   , m_width( size.x )
   , m_height( size.y )
{
   setSize( m_width, m_height );
   generateExits( arrExitDirections );
   populateWithFurniture();
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoom::~GL2DRoom()
{
   uint areasCount = m_areas.size();
   for ( uint i = 0; i < areasCount; ++i )
   {
      GL2DRoomArea* area = m_areas[i];
      delete area;
   }
   m_areas.clear();
   m_connectivityPoint.clear();
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::setSize( byte width, byte height)
{
   m_width = width;
   m_height = height;

   m_areas.resize( m_width * m_height, NULL );

   Point areaPos;
   uint areaIdx = 0;
   for ( areaPos.y = 0; areaPos.y < ( int ) m_height; ++areaPos.y )
   {
      const bool topBoundary = ( areaPos.y == 0 );
      const bool bottomBoundary = ( areaPos.y == m_height - 1 );

      for ( areaPos.x = 0; areaPos.x < ( int ) m_width; ++areaPos.x, ++areaIdx )
      {
         if ( m_areas[areaIdx] != NULL )
         {
            // this area already exists - move on to the next
            continue;
         }
         const bool leftBoundary = ( areaPos.x == 0 );
         const bool rightBoundary = ( areaPos.x == m_width - 1 );

         GL2DRoomArea* area = new GL2DRoomArea( areaPos, topBoundary, bottomBoundary, leftBoundary, rightBoundary );
         m_areas[areaIdx] = area;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::generateExits( const Array< GL2DVoxelRotation >& arrExitDirections )
{
   const Point wallOrigin[4] = { Point( 0, 0 ), Point( m_width - 1, 0 ), Point( m_width - 1, m_height - 1 ), Point( 0, m_height - 1 ) };
   const Point wallDir[4] = { Point( 1, 0 ), Point( 0, 1 ), Point( -1, 0 ), Point( 0, -1 ) };
   const uint wallLenght[4] = { m_width - 1, m_height - 1, m_width - 1, m_height - 1 };

   const uint exitCount = arrExitDirections.size();
   for ( uint i = 0; i < exitCount; ++i )
   {
      const byte side = ( byte ) arrExitDirections[i];

      while ( true )
      {
         const Point pos = wallOrigin[side] + wallDir[side] *( rand() % wallLenght[side] );
         const uint areaAddr = calcAreaAddr( pos );

         GL2DRoomArea* selectedArea = m_areas[areaAddr];
         if ( selectedArea->getGeometryItem() != NULL )
         {
            // this area's already occupied
            continue;
         }
         selectedArea->setGeometryItem( m_theme, true );

         if ( ListUtils::find( m_connectivityPoint, selectedArea ).isEnd() )
         {
            // record the connectivity point
            m_connectivityPoint.pushBack( m_areas[areaAddr] );
            break;
         }
      }
   }

   // fill the remaining areas with walls
   const uint areasCount = m_areas.size();
   for ( uint i = 0; i < areasCount; ++i )
   {
      GL2DRoomArea* area = m_areas[i];
      if ( area->getGeometryItem() == NULL )
      {
         area->setGeometryItem( m_theme, false );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::getBounds( GL2DBounds& outBounds, int inflationRadius ) const
{
   // pathfinding coordinates

   // inflate the bounds a bit - we don't want the rooms
   // leaning on one another
   outBounds.set( m_pos + Point( -inflationRadius, -inflationRadius ), m_pos + Point( m_width + inflationRadius, m_height + inflationRadius ) );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::setPosition( const Point& pos )
{
   // pathfinding coordinates
   m_pos = pos;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::translate( const Point& offset )
{
   // pathfinding coordinates
   m_pos += offset;
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DRoom::doesOverlap( const Array< GL2DRoom* >& rooms ) const
{
   GL2DBounds bounds, otherBounds;
   getBounds( bounds, 1 );

   const uint count = rooms.size();
   for ( uint i = 0; i < count; ++i )
   {
      GL2DRoom* otherRoom = rooms[i];
      if ( !otherRoom || otherRoom == this )
      {
         continue;
      }
      otherRoom->getBounds( otherBounds, 1 );

      if ( bounds.testIntersection( otherBounds ) )
      {
         return true;
      }
   }
   return false;
}

///////////////////////////////////////////////////////////////////////////////

float GL2DRoom::getNearestExits( const GL2DRoom* otherRoom, Point& outCorridorStartPos, Point& outCorridorEndPos ) const
{
   // search for the nearest exits
   const uint thisCount = m_connectivityPoint.size();
   const uint rhsCount = otherRoom->m_connectivityPoint.size();
   Point pos1, pos2;
   float minDist = FLT_MAX;

   for ( List< GL2DRoomArea* >::const_iterator thisIt = m_connectivityPoint.begin(); !thisIt.isEnd(); ++thisIt )
   {
      GL2DRoomArea* thisPt = *thisIt;
      thisPt->getConnectorPos( pos1 );
      pos1 += m_pos;

      for ( List< GL2DRoomArea* >::const_iterator rhsIt = otherRoom->m_connectivityPoint.begin(); !rhsIt.isEnd(); ++rhsIt )
      {
         GL2DRoomArea* rhsPt = *rhsIt;
         rhsPt->getConnectorPos( pos2 );
         pos2 += otherRoom->m_pos;


         const float dist = pos1.distanceSqTo( pos2 );
         if ( dist < minDist )
         {
            minDist = dist;
            outCorridorStartPos = pos1;
            outCorridorEndPos = pos2;
         }
      }
   }

   return minDist;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::connectToRoom( GL2DRoom* otherRoom )
{
   // record the fact that the two rooms are now considered connected to one another
   m_connectedRooms.pushBack( otherRoom );
   otherRoom->m_connectedRooms.pushBack( this );
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DRoom::isConnectedTo( const GL2DRoom* otherRoom ) const
{
   List< const GL2DRoom* >::const_iterator it = ListUtils::find( m_connectedRooms, otherRoom );
   return !it.isEnd();
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::addFurniture( Prefab* prefab )
{
   if ( prefab )
   {
      m_requiredFurniture.pushBack( prefab );
   }
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoom::LayoutInfo::LayoutInfo( uint width, uint height )
   : m_wallIdx( 0 )
{
   m_wallOrigin[0].set( 0, 0 );
   m_wallOrigin[1].set( width - 1, 0 );
   m_wallOrigin[2].set( width - 1, height - 1 );
   m_wallOrigin[3].set( 0, height - 1 );

   m_wallDir[0].set( 1, 0 );
   m_wallDir[1].set( 0, 1 );
   m_wallDir[2].set( -1, 0 );
   m_wallDir[3].set( 0, -1 );

   m_wallOffset[0] = 0; m_wallOffset[1] = width - 1;
   m_wallOffset[2] = 0; m_wallOffset[3] = height - 1;
   m_wallOffset[4] = 0; m_wallOffset[5] = width - 1;
   m_wallOffset[6] = 0; m_wallOffset[7] = height - 1;

   memset( m_fromFront, 0, sizeof( byte ) * 4 );
}

void GL2DRoom::populateWithFurniture()
{
   int itemsRemaining = (uint)ceil( (float)( m_width * m_height ) / 4.f );
   LayoutInfo layout( m_width, m_height );

   // add the required furniture
   for ( List< Prefab* >::iterator it = m_requiredFurniture.begin(); !it.isEnd(); ++it )
   {
      Prefab* prefab = *it;

      addToLayout( prefab, layout );
      --itemsRemaining;
   }

   // fill the remaining free space with random stuff
   for ( ; itemsRemaining >= 0; --itemsRemaining )
   {
      const Prefab* furniturePrefab = m_theme->getRandomFurniturePrefab();
      addToLayout( furniturePrefab, layout );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::addToLayout( const Prefab* prefab, LayoutInfo& layout )
{
   bool wasAdded = false;
   byte wallsTested = 0;
   while ( !wasAdded && wallsTested < 4 )
   {
      // does this wall have any more open spots
      const byte offsetAddr = layout.m_wallIdx * 2 + layout.m_fromFront[layout.m_wallIdx];
      if ( layout.m_wallOffset[layout.m_wallIdx * 2] < layout.m_wallOffset[layout.m_wallIdx * 2 + 1] )
      {
         const Point areaPos = layout.m_wallOrigin[layout.m_wallIdx] + layout.m_wallDir[layout.m_wallIdx] * layout.m_wallOffset[offsetAddr];
         const uint areaAddr = calcAreaAddr( areaPos );

         GL2DRoomArea* selectedArea = m_areas[areaAddr];
         wasAdded = selectedArea->setDecorationItem( prefab );
      }

      layout.m_wallOffset[offsetAddr] += ( layout.m_fromFront[layout.m_wallIdx] == 0 ? 1 : -1 );
      layout.m_fromFront[layout.m_wallIdx] = 1 - layout.m_fromFront[layout.m_wallIdx];
      layout.m_wallIdx = randExclusive( 4, layout.m_wallIdx );
      ++wallsTested;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::voxelizeForPathfinding( Grid< uint >& pathfindingGrid ) const
{
   // pathfinding coordinates

   Point pos, start, end;
   start = m_pos;
   end = m_pos + Point( m_width, m_height );
   for ( pos.y = m_pos.y; pos.y < end.y; ++pos.y )
   {
      for ( pos.x = m_pos.x; pos.x < end.x; ++pos.x )
      {
         const uint addr = pathfindingGrid.calcAddr( pos );
         pathfindingGrid[addr] = PF_ROOM_FLOOR_ID;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoom::render( uint voxelsPerUnit, StaticGeometryTree& staticGeometryBuilder ) const
{
   // voxelize the walls
   const uint areasCount = m_areas.size();
   for ( uint i = 0; i < areasCount; ++i )
   {
      GL2DRoomArea* area = m_areas[i];
      area->render( m_pos, voxelsPerUnit, staticGeometryBuilder );
   }
}

///////////////////////////////////////////////////////////////////////////////
