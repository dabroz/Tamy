#include "ext-2DGameLevel\GL2DRoomLayout.h"
#include "ext-2DGameLevel\GL2DRoom.h"
#include "ext-2DGameLevel\GL2DCorridor.h"
#include "ext-2DGameLevel\GL2DLevelVoxels.h"
#include "ext-2DGameLevel\GL2DBounds.h"
#include "ext-2DGameLevel\GL2DLevelGenerator.h"

// utils
#include "core\Point.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

GL2DRoomLayoutLeaf::GL2DRoomLayoutLeaf( GL2DRoomTheme* theme, GL2DRoomSize sizeHint )
{
   const Point roomSizes[] = {
      Point( 2, 2 ),          // GL2D_Room_Small
      Point( 3, 2 ),          // GL2D_Room_Medium
      Point( 3, 3 ),          // GL2D_Room_Large
   };

   Array< GL2DVoxelRotation > doorOrientation;
   doorOrientation.push_back( VRot_180 );

   const Point& size = roomSizes[sizeHint];
   Point offset;
   for ( uint i = 0; i < 3; ++i )
   {
      GL2DRoom* room = new GL2DRoom( theme, size, doorOrientation );
      room->setPosition( offset );
      m_rooms.push_back( room );

      offset.x += size.x;
   }

   // create a corridor
   m_corridor = new GL2DCorridor( theme );
   m_corridor->drawStraightLine( Point( 0, size.y ), Point( offset.x, size.y ) );
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoomLayoutLeaf::~GL2DRoomLayoutLeaf()
{
   const uint count = m_rooms.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_rooms[i];
   }
   m_rooms.clear();

   delete m_corridor;
   m_corridor = NULL;
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoom* GL2DRoomLayoutLeaf::getRoom( uint idx ) const
{
   const uint roomIdx = clamp<uint>( idx, 0, m_rooms.size() - 1 );
   return m_rooms[roomIdx];
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomLayoutLeaf::build( GL2DLevelGenerator& generator )
{
   const uint count = m_rooms.size();
   for ( uint i = 0; i < count; ++i )
   {
      generator.addRoom( m_rooms[i] );
   }

   generator.addCorridor( m_corridor );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomLayoutLeaf::translate( const Point& offset )
{
   const uint count = m_rooms.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_rooms[i]->translate( offset );
   }

   m_corridor->translate( offset );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomLayoutLeaf::getBounds( GL2DBounds& outBounds ) const
{
   GL2DBounds roomBounds;

   const uint count = m_rooms.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_rooms[i]->getBounds( roomBounds );

      outBounds.add( roomBounds );
   }

   m_corridor->getBounds( outBounds );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GL2DRoomLayoutComposite::GL2DRoomLayoutComposite( GL2DRoomTheme* theme )
   : m_corridor( new GL2DCorridor( theme ) )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoomLayoutComposite::~GL2DRoomLayoutComposite()
{
   const uint count = m_layout.size();
   for ( uint i = 0; i < count; ++i )
   {
      delete m_layout[i];
   }
   m_layout.clear();

   delete m_corridor;
   m_corridor = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomLayoutComposite::addLayout( GL2DRoomLayout* layout )
{
   if ( !m_layout.empty() )
   {
      GL2DBounds thisLayoutBounds;
      getBounds( thisLayoutBounds );
      Point offsetMin, offsetMax;
      thisLayoutBounds.get( offsetMin, offsetMax );

      offsetMax.x = 0;
      layout->translate( offsetMax );
   }
  
   m_layout.push_back( layout );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomLayoutComposite::build( GL2DLevelGenerator& generator )
{
   const uint count = m_layout.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_layout[i]->build( generator );
   }

   // create a corridor that spans along the other layouts
   GL2DBounds bounds;
   getBounds( bounds );
   Point boundsMin, boundsMax;
   bounds.get( boundsMin, boundsMax );
   boundsMax.x -= 1;
   boundsMin.x = boundsMax.x;

   m_corridor->drawStraightLine( boundsMin, boundsMax );
   generator.addCorridor( m_corridor );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomLayoutComposite::translate( const Point& offset )
{
   const uint count = m_layout.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_layout[i]->translate( offset );
   }

   m_corridor->translate( offset );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomLayoutComposite::getBounds( GL2DBounds& outBounds ) const
{
   GL2DBounds bounds; 
   const uint count = m_layout.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_layout[i]->getBounds( bounds );
   }

   // add some room for the corridor
   Point boundsMin, boundsMax;
   bounds.get( boundsMin, boundsMax );

   boundsMax += Point( 1, 0 );
   bounds.set( boundsMin, boundsMax );
   outBounds.add( bounds );
}

///////////////////////////////////////////////////////////////////////////////
