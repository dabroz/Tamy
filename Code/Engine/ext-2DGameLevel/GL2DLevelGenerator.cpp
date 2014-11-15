#include "ext-2DGameLevel\GL2DLevelGenerator.h"
#include "ext-2DGameLevel\GL2DVoxelPrefabsMap.h"
#include "ext-2DGameLevel\GL2DLSystem.h"

// level rendering
#include "core-MVC\Entity.h"
#include "core-Renderer\StaticGeometryTree.h"
#include "core-Renderer\RenderSystem.h"

// logging
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

GL2DLevelGenerator::GL2DLevelGenerator( const FilePath& lSystemDir, const FilePath& geometryDeploymentDir )
   : m_lSystemDir( lSystemDir )
   , m_geometryDeploymentDir( geometryDeploymentDir )
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


   FilePath lSystemConfigXML = m_lSystemDir + FilePath( "LSystemDef.xml" );
   GL2DLSystem lSystem;
   if ( !lSystem.configureFromXML( lSystemConfigXML ) )
   {
      return NULL;
   }

   // run the generator
   std::string levelDefinition;
   lSystem.process( "a", 5, levelDefinition );

   List< Prefab* > prefabsList;
   lSystem.interpret( levelDefinition, m_lSystemDir, prefabsList );

   Matrix transform;
   float runningLength = 0.0f;
   for ( List< Prefab* >::iterator it = prefabsList.begin(); !it.isEnd(); ++it )
   {
      const Prefab* prefab = *it;

      // calculate the object's transform
      transform.setTranslation( Vector( runningLength, 0.0f, 0.0f ) );
      staticGeometryBuilder.add( prefab, transform );

      runningLength += 2.0f;
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