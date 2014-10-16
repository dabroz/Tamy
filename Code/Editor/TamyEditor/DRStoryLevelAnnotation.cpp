#include "DRStoryLevelAnnotation.h"
#include "ext-StoryTeller\StoryLevelAnnotation.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\TriangleMesh.h"
#include "DebugGeometryBuilder.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( DRStoryLevelAnnotation )
   PARENT( DebugGeometry )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

DRStoryLevelAnnotation::DRStoryLevelAnnotation( StoryLevelAnnotation& annotation )
   : DebugGeometry( &annotation )
   , m_annotation( annotation )
   , m_mesh( NULL )
{
   m_mesh = DebugGeometryBuilder::createDiamond( 0.1f, Quad_0010, Quad_0 );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Material* material = resMgr.create< Material >( FilePath( "/Editor/Materials/DirectionalLight.tmat" ), false );

   m_geomComp = new DebugGeometryComponent( *m_mesh );
   m_geomComp->setMaterial( material );
   addChild( m_geomComp );
}

///////////////////////////////////////////////////////////////////////////////

DRStoryLevelAnnotation::~DRStoryLevelAnnotation()
{
   m_mesh->removeReference();
   m_mesh = NULL;

   m_geomComp = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DRStoryLevelAnnotation::enableBoundingBox( bool enable )
{
}

///////////////////////////////////////////////////////////////////////////////

void DRStoryLevelAnnotation::enableDebugShape( bool enable )
{
   m_geomComp->enableRendering( enable );
}

///////////////////////////////////////////////////////////////////////////////

void DRStoryLevelAnnotation::updateDebugData( float timeElapsed )
{
   m_geomComp->setExtraTransform( getGlobalMtx() );
}

///////////////////////////////////////////////////////////////////////////////
