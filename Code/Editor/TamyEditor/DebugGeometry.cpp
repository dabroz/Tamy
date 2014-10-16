#include "DebugGeometry.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\Defines.h"
#include "core-Renderer\GeometryResource.h"
#include "core-Renderer\RenderState.h"
#include "core\ResourcesManager.h"
#include "core\FilePath.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( DebugGeometry )
   PARENT( Entity )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

DebugGeometry::DebugGeometry( SceneNode* representedNodePtr )
   : m_representedNodePtr( representedNodePtr )
{
}

///////////////////////////////////////////////////////////////////////////////

DebugGeometry::~DebugGeometry()
{
   m_representedNodePtr = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( DebugGeometryComponent )
   PARENT( GeometryComponent )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

DebugGeometryComponent::DebugGeometryComponent()
   : m_extraTransform( Matrix::IDENTITY )
{
   // debug geometry doesn't cast shadows
   setShadowcaster( false );
}

///////////////////////////////////////////////////////////////////////////////

DebugGeometryComponent::DebugGeometryComponent( GeometryResource& mesh )
   : GeometryComponent( mesh )
   , m_extraTransform( Matrix::IDENTITY )
{
   // debug geometry doesn't cast shadows
   setShadowcaster( false );
}

///////////////////////////////////////////////////////////////////////////////
