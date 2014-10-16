#include "DebugGrid.h"
#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\BasicRenderCommands.h"
#include "core-Renderer\LitVertex.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Defines.h"
#include "DebugGeometryBuilder.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT(DebugGrid)
   PARENT( DebugGeometry )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

DebugGrid::DebugGrid()
   : DebugGeometry( NULL )
{
   m_mesh = DebugGeometryBuilder::createPlane( 1000, 1000, 10, 10 );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Material* material = resMgr.create< Material >( FilePath( "/Editor/Materials/Grid.tmat" ), false );

   m_geomComp = new DebugGeometryComponent( *m_mesh );
   addChild( m_geomComp );

   m_geomComp->setMaterial( material );
}

///////////////////////////////////////////////////////////////////////////////

DebugGrid::~DebugGrid()
{
   m_mesh->removeReference();
   m_mesh = NULL;

   m_geomComp = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DebugGrid::show( bool enable )
{
   m_geomComp->enableRendering( enable );
}

///////////////////////////////////////////////////////////////////////////////
