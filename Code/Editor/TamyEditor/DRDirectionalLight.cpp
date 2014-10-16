#include "DRDirectionalLight.h"
#include "core-Renderer\DirectionalLight.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\TriangleMesh.h"
#include "DebugGeometryBuilder.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( DRDirectionalLight )
   PARENT( DebugGeometry )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

DRDirectionalLight::DRDirectionalLight( DirectionalLight& light )
   : DebugGeometry( &light )
   , m_light( light )
   , m_mesh( NULL )
{
   m_mesh = DebugGeometryBuilder::createArrow( 0.2f, Vector( 0.0f, 0.0f, 0.5f ), Vector( 0.0f, 0.0f, -0.5f ) );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Material* material = resMgr.create< Material >( FilePath( "/Editor/Materials/DirectionalLight.tmat" ), false );
   
   m_geomComp = new DebugGeometryComponent( *m_mesh );
   m_geomComp->setMaterial( material );
   addChild( m_geomComp );
}

///////////////////////////////////////////////////////////////////////////////

DRDirectionalLight::~DRDirectionalLight()
{
   m_mesh->removeReference();
   m_mesh = NULL;

   m_geomComp = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DRDirectionalLight::enableBoundingBox( bool enable )
{
}

///////////////////////////////////////////////////////////////////////////////

void DRDirectionalLight::enableDebugShape( bool enable )
{
   m_geomComp->enableRendering( enable );
}

///////////////////////////////////////////////////////////////////////////////

void DRDirectionalLight::updateDebugData( float timeElapsed )
{
   m_geomComp->setExtraTransform( getGlobalMtx() );
}

///////////////////////////////////////////////////////////////////////////////
