#include "ext-2DGameLevel\GL2DLevelGenerator.h"
#include "ext-2DGameLevel\GL2DVoxelPrefabsMap.h"

// level rendering
#include "core-MVC\Entity.h"
#include "core-Renderer\StaticGeometryTree.h"
#include "core-Renderer\RenderSystem.h"

// logging
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

GL2DLevelGenerator::GL2DLevelGenerator( const FilePath& geometryDeploymentDir, const GL2DVoxelPrefabsMap& prefabsMap )
   : m_geometryDeploymentDir( geometryDeploymentDir )
   , m_prefabsMap( prefabsMap )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DLevelGenerator::~GL2DLevelGenerator()
{
}

///////////////////////////////////////////////////////////////////////////////

Entity* GL2DLevelGenerator::generateLevel( float levelLength )
{
   StaticGeometryTree staticGeometryBuilder( AxisAlignedBox( Vector( -1.5f * levelLength, -10.0f, -10.0f ), Vector( 1.5f * levelLength, 10.0f, 10.0f ) ) );

   const uint prefabsCount = m_prefabsMap.getPrefabsCount();
   if ( prefabsCount == 0 )
   {
      WARNING( "No prefabs have been found" );
      return NULL;
   }

   Matrix transform;
   for ( float runningLength = 0.0f; runningLength < levelLength; runningLength += 2.0f )
   {
      const uint prefabIdx = rand() % prefabsCount;
      const Prefab* randomPrefab = m_prefabsMap.getPrefab( prefabIdx );

      transform.setTranslation( Vector( runningLength, 0.0f, 0.0f ) );

      staticGeometryBuilder.add( randomPrefab, transform );
   }

   Entity* proceduralGeometry = staticGeometryBuilder.build( m_geometryDeploymentDir, "ProceduralLevel" );

   // This operation could potentially generate some render commands related to 
   // rebuilding the geometry of the existing triangle meshes - so we need to
   // force-flush the rendering thread
   RenderSystem& renderSystem = TSingleton< RenderSystem >::getInstance();
   renderSystem.flush();

   return proceduralGeometry;
}

///////////////////////////////////////////////////////////////////////////////