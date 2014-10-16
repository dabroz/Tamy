#include "DRSpotLight.h"
#include "core-Renderer\SpotLight.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\TriangleMesh.h"
#include "DebugGeometryBuilder.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( DRSpotLight )
   PARENT( DebugGeometry )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

DRSpotLight::DRSpotLight( SpotLight& light )
   : DebugGeometry( &light )
   , m_light( light )
   , m_mesh( NULL )
{
   Vector endPos( 0.0f, 0.0f, -0.5f );
   m_mesh = DebugGeometryBuilder::createDiamond( 0.2f, Quad_0, endPos );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Material* material = resMgr.create< Material >( FilePath( "/Editor/Materials/DirectionalLight.tmat" ), false );

   m_geomComp = new DebugGeometryComponent( *m_mesh );
   m_geomComp->setMaterial( material );
   addChild( m_geomComp );
}

///////////////////////////////////////////////////////////////////////////////

DRSpotLight::~DRSpotLight()
{
   m_mesh->removeReference();
   m_mesh = NULL;

   m_geomComp = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DRSpotLight::enableBoundingBox( bool enable )
{
}

///////////////////////////////////////////////////////////////////////////////

void DRSpotLight::enableDebugShape( bool enable )
{
   m_geomComp->enableRendering( enable );
}

///////////////////////////////////////////////////////////////////////////////

void DRSpotLight::updateDebugData( float timeElapsed )
{
   m_geomComp->setExtraTransform( getGlobalMtx() );
}

///////////////////////////////////////////////////////////////////////////////
