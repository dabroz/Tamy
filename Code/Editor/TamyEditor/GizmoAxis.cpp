#include "GizmoAxis.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\BasicRenderCommands.h"
#include "core-Renderer\LitVertex.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Defines.h"
#include "core\Math.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( GizmoAxis );
   PARENT( DebugGeometry );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

GizmoAxis::GizmoAxis( byte axisIdx, Entity& editedNode )
   : DebugGeometry( NULL )
   , m_axisIdx( axisIdx )
{
   setRepresentedNodePtr( this );

   m_geomComp = new GizmoAxisGeometryComponent( editedNode );
   addChild( m_geomComp );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   Material* material = NULL;
   switch( axisIdx )
   {
   case 0:
      {
         material = resMgr.create< Material >( FilePath( "/Editor/Materials/GizmoOXAxis.tmat" ), true );
         break;
      }

   case 1:
      {
         material = resMgr.create< Material >( FilePath( "/Editor/Materials/GizmoOYAxis.tmat" ), true );
         break;
      }

   case 2:
      {
         material = resMgr.create< Material >( FilePath( "/Editor/Materials/GizmoOZAxis.tmat" ), true );
         break;
      }
   }

   m_geomComp->setMaterial( material );
}

///////////////////////////////////////////////////////////////////////////////

GizmoAxis::~GizmoAxis()
{
}

///////////////////////////////////////////////////////////////////////////////

void GizmoAxis::setMesh( GeometryResource& mesh )
{
   m_geomComp->setMesh( mesh );
}

///////////////////////////////////////////////////////////////////////////////

void GizmoAxis::show( bool visible )
{
   m_geomComp->enableRendering( visible );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( GizmoAxisGeometryComponent );
   PARENT( GeometryComponent );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

GizmoAxisGeometryComponent::GizmoAxisGeometryComponent( Entity& editedNode )
   : m_editedNode( editedNode )
{
}

///////////////////////////////////////////////////////////////////////////////
