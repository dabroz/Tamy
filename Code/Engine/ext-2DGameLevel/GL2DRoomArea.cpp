#include "ext-2DGameLevel\GL2DRoomArea.h"
#include "ext-2DGameLevel\GL2DVoxelizedItem.h"
#include "ext-2DGameLevel\GL2DLevelVoxels.h"
#include "ext-2DGameLevel\GL2DWallVoxel.h"
#include "ext-2DGameLevel\GL2DRoomTheme.h"


///////////////////////////////////////////////////////////////////////////////

GL2DRoomArea::GL2DRoomArea( const Point& offset, bool topWall, bool bottomWall, bool leftWall, bool rightWall )
   : m_topWall( topWall )
   , m_bottomWall( bottomWall )
   , m_leftWall( leftWall )
   , m_rightWall( rightWall ) 
   , m_offset( offset )
   , m_door( false )
   , m_geometryItem( NULL )
   , m_decorationItem( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DRoomArea::~GL2DRoomArea()
{
   m_geometryItem = NULL;

   delete m_decorationItem;
   m_decorationItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomArea::setGeometryItem( const GL2DRoomTheme* theme, bool door )
{
   m_door = door;

   if ( !m_topWall && !m_bottomWall && !m_leftWall && !m_rightWall )
   {
      m_geometryItem = theme->getCoreItem( LVP_FLOOR );
      return;
   }

   const byte typeOffset = door ? ( LVP_DOOR_TOP - LVP_WALL_TOP ) : 0;

   // corners
   if ( m_topWall && m_leftWall )
   {
      m_geometryItem = theme->getCoreItem( ( CoreVoxelItemsIds )( LVP_WALL_CORNER_TOP_LEFT + typeOffset ) );
      return;
   }

   if ( m_topWall && m_rightWall )
   {
      m_geometryItem = theme->getCoreItem( ( CoreVoxelItemsIds ) ( LVP_WALL_CORNER_TOP_RIGHT + typeOffset ) );
      return;
   }

   if ( m_bottomWall && m_leftWall )
   {
      m_geometryItem = theme->getCoreItem( ( CoreVoxelItemsIds ) ( LVP_WALL_CORNER_BOTTOM_LEFT + typeOffset ) );
      return;
   }

   if ( m_bottomWall && m_rightWall )
   {
      m_geometryItem = theme->getCoreItem( ( CoreVoxelItemsIds ) ( LVP_WALL_CORNER_BOTTOM_RIGHT + typeOffset ) );
      return;
   }

   // straight segments
   if ( m_topWall )
   {
      m_geometryItem = theme->getCoreItem( ( CoreVoxelItemsIds ) ( LVP_WALL_TOP + typeOffset ) );
      return;
   }

   if ( m_rightWall )
   {
      m_geometryItem = theme->getCoreItem( ( CoreVoxelItemsIds ) ( LVP_WALL_RIGHT + typeOffset ) );
      return;
   }

   if ( m_bottomWall )
   {
      m_geometryItem = theme->getCoreItem( ( CoreVoxelItemsIds ) ( LVP_WALL_BOTTOM + typeOffset ) );
      return;
   }

   if ( m_leftWall )
   {
      m_geometryItem = theme->getCoreItem( ( CoreVoxelItemsIds ) ( LVP_WALL_LEFT + typeOffset ) );
      return;
   }
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DRoomArea::setDecorationItem( const Prefab* itemPrefab )
{
   if ( m_decorationItem || m_door )
   {
      return false;
   }

   m_decorationItem = new GL2DVoxelizedItem( itemPrefab, VRot_0 );
   updateRotationForAlignment();

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomArea::updateRotationForAlignment()
{
   if ( m_topWall )
   {
      m_decorationItem->setRotation( VRot_0 );
   }
   else if ( m_bottomWall )
   {
      m_decorationItem->setRotation( VRot_180 );
   }
   else if ( m_leftWall )
   {
      m_decorationItem->setRotation( VRot_270 );
   }
   else if ( m_rightWall )
   {
      m_decorationItem->setRotation( VRot_90 );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomArea::getConnectorPos( Point& outPos ) const
{
   if ( m_topWall )
   {
      outPos = m_offset + Point( 0, -1 );
   }
   else if ( m_bottomWall )
   {
      outPos = m_offset + Point( 0, 1 );
   }
   else if ( m_leftWall )
   {
      outPos = m_offset + Point( -1, 0 );
   }
   else if ( m_rightWall )
   {
      outPos = m_offset + Point( 1, 0 );
   }
   else
   {
      ASSERT_MSG( false, "This area can't be used as a connectivity point" );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DRoomArea::render( const Point& parentTranslation, uint voxelsPerUnit, StaticGeometryTree& staticGeometryBuilder ) const
{
   const Point areaRenderingPos = ( parentTranslation + m_offset ) * voxelsPerUnit;

   if ( m_geometryItem )
   {
      m_geometryItem->render( areaRenderingPos, staticGeometryBuilder );
   }

   if ( m_decorationItem )
   {
      m_decorationItem->render( areaRenderingPos, staticGeometryBuilder );
   }
}

///////////////////////////////////////////////////////////////////////////////
