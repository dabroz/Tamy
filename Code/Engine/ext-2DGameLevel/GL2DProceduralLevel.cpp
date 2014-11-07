#include "ext-2DGameLevel\GL2DProceduralLevel.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GL2DProceduralLevel );
   PARENT( Component );
   PROPERTY_EDIT( "Procedural geometry deployment dir", FilePath, m_geometryDir );
   PROPERTY_EDIT( "Background voxels dir", FilePath, m_backgroundPrefabsDir );
   PROPERTY_EDIT( "Level length", float, m_levelLenght );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

GL2DProceduralLevel::GL2DProceduralLevel( const char* name )
   : Component( name )
   , m_levelLenght( 10.0f )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DProceduralLevel::GL2DProceduralLevel( const GL2DProceduralLevel& rhs )
   : Component( rhs )
   , m_backgroundPrefabsDir( rhs.m_backgroundPrefabsDir )
   , m_levelLenght( rhs.m_levelLenght )
{
}

///////////////////////////////////////////////////////////////////////////////
