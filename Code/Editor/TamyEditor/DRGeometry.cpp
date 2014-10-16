#include "DRGeometry.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\TriangleMesh.h"
#include "core-MVC\Entity.h"
#include "core\ReflectionProperty.h"
#include "core\AxisAlignedBox.h"
#include "DebugGeometryBuilder.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( DRGeometry )
   PARENT( DebugGeometry )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

DRGeometry::DRGeometry( Entity& entity )
   : DebugGeometry( &entity )
   , m_entity( entity )
   , m_mesh( NULL )
   , m_visible( false )
{
   const AxisAlignedBox& bb = entity.getBoundingVolume();

   m_mesh = DebugGeometryBuilder::createBox( Vector( -0.5f, -0.5f, 0.5f ), Vector( 0.5f, 0.5f, 0.5f ) );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Material* material = resMgr.create< Material >( FilePath( "/Editor/Materials/DebugGeometry.tmat" ), false );

   m_geomComp = new DebugGeometryComponent( *m_mesh );
   m_geomComp->setMaterial( material );
   addChild( m_geomComp );

   // don't attach the material just yet, as this debug representation is invisible by default
}

///////////////////////////////////////////////////////////////////////////////

DRGeometry::~DRGeometry()
{
   m_mesh->removeReference();
   m_mesh = NULL;

   m_geomComp = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DRGeometry::enableBoundingBox( bool enable )
{
   m_visible = enable;
   m_geomComp->enableRendering( enable );
}

///////////////////////////////////////////////////////////////////////////////

void DRGeometry::enableDebugShape( bool enable )
{
}

///////////////////////////////////////////////////////////////////////////////

void DRGeometry::updateDebugData( float timeElapsed )
{
   if ( !m_visible )
   {
      const AxisAlignedBox& currentBounds = m_entity.getBoundingVolume();
      
      Vector translation, extents;
      currentBounds.getCenter( translation );
      currentBounds.getExtents( extents );

      Matrix scaleTransform;
      scaleTransform.scale( extents );

      Matrix translationTransform;
      translationTransform.setTranslation( translation );

      Matrix bbTransform;
      bbTransform.setMul( scaleTransform, translationTransform );

      m_geomComp->setExtraTransform( bbTransform );
   }
}

///////////////////////////////////////////////////////////////////////////////
