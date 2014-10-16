#include "DRPointLight.h"
#include "core-Renderer/PointLight.h"
#include "core-Renderer/GeometryComponent.h"
#include "core-Renderer/TriangleMesh.h"
#include "DebugGeometryBuilder.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( DRPointLight )
   PARENT( DebugGeometry )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

DRPointLight::DRPointLight( PointLight& light )
   : DebugGeometry( &light )
   , m_light( light )
   , m_mesh( NULL )
{
   m_mesh = DebugGeometryBuilder::createTorus( 0.5f, 1.0f, Matrix::IDENTITY, 6, 5 );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Material* material = resMgr.create< Material >( FilePath( "/Editor/Materials/PointLight.tmat" ), false );

   m_geomComp = new DebugGeometryComponent( *m_mesh );
   addChild( m_geomComp );

   m_geomComp->setMaterial( material );
}

///////////////////////////////////////////////////////////////////////////////

DRPointLight::~DRPointLight()
{
   m_mesh->removeReference();
   m_mesh = NULL;

   m_geomComp = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DRPointLight::enableBoundingBox( bool enable )
{
}

///////////////////////////////////////////////////////////////////////////////

void DRPointLight::enableDebugShape( bool enable )
{
   m_geomComp->enableRendering( enable );
}

///////////////////////////////////////////////////////////////////////////////

void DRPointLight::updateDebugData( float timeElapsed )
{
   m_geomComp->setExtraTransform( getGlobalMtx() );
}

///////////////////////////////////////////////////////////////////////////////
