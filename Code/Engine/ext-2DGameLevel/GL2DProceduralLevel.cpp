#include "ext-2DGameLevel\GL2DProceduralLevel.h"
#include "core\Point.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GL2DProceduralLevel );
   PARENT( Component );
   PROPERTY_EDIT( "Geometry voxels dir", FilePath, m_geometryVoxelsDir );
   PROPERTY_EDIT( "Deployment dir", FilePath, m_geometryDeploymentDir );
   PROPERTY_EDIT( "Voxels per cell", uint, m_voxelsPerCell );
   PROPERTY_EDIT( "Rooms count", uint, m_roomsCount );
   PROPERTY_EDIT( "Side corridors depth", uint, m_maxSideCorridorsDepth );
   PROPERTY_EDIT( "Max expanded side rooms", uint, m_maxExpandedChildrenCount );
   PROPERTY_EDIT( "Corridor type", CostFunctionEnum, m_corridorType );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( CostFunctionEnum );
   ENUM_VAL( GL2D_StraightCorridors );
   ENUM_VAL( GL2D_BendyCorridors );
END_ENUM();

///////////////////////////////////////////////////////////////////////////////

GL2DProceduralLevel::GL2DProceduralLevel( const char* name )
   : Component( name )
   , m_voxelsPerCell( 5 )
   , m_roomsCount( 5 )
   , m_maxSideCorridorsDepth( 1 )
   , m_maxExpandedChildrenCount( 2 )
   , m_corridorType( GL2D_StraightCorridors )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DProceduralLevel::GL2DProceduralLevel( const GL2DProceduralLevel& rhs )
   : Component( rhs )
   , m_geometryVoxelsDir( rhs.m_geometryVoxelsDir )
   , m_geometryDeploymentDir( rhs.m_geometryDeploymentDir )
   , m_voxelsPerCell( rhs.m_voxelsPerCell )
   , m_roomsCount( rhs.m_roomsCount )
   , m_maxSideCorridorsDepth( rhs.m_maxSideCorridorsDepth )
   , m_maxExpandedChildrenCount( rhs.m_maxExpandedChildrenCount )
   , m_corridorType( rhs.m_corridorType )
{
}

///////////////////////////////////////////////////////////////////////////////

static uint monacoDistCostFunc( const Point& start, const Point& end )
{
   return abs( end.x - start.x ) + abs( end.y - start.y );
}

///////////////////////////////////////////////////////////////////////////////

static uint cartesianDistCostFunc( const Point& start, const Point& end )
{
   return (uint)start.distanceSqTo( end );
}

///////////////////////////////////////////////////////////////////////////////

const CostFunc g_constFunctions [] = { &monacoDistCostFunc, &cartesianDistCostFunc };

CostFunc GL2DProceduralLevel::getCostFunction() const
{
   return g_constFunctions[m_corridorType];
   
}

///////////////////////////////////////////////////////////////////////////////
