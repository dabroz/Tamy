#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\GeometryResource.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Shader.h"
#include "core-MVC\Entity.h"
#include "core\ReflectionProperty.h"
#include "core\ListUtils.h"
#include "core\ArrayUtils.h"
#include "core\AxisAlignedBox.h"
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GeometryComponent );
   PARENT( Component );
   PROPERTY_EDIT( "resource", GeometryResource*, m_resource );
   PROPERTY_EDIT( "Shadow caster", bool, m_shadowCaster );
   PROPERTY( RenderState*, m_renderState );
END_OBJECT();


///////////////////////////////////////////////////////////////////////////////

GeometryComponent::GeometryComponent( const char* name )
   : Component( name )
   , m_resource( NULL )
   , m_renderState( new RenderState() )
   , m_enabled( true )
   , m_shadowCaster( true )
{
}

///////////////////////////////////////////////////////////////////////////////

GeometryComponent::GeometryComponent( const GeometryComponent& rhs )
   : Component( rhs )
   , m_resource( rhs.m_resource )
   , m_boundingVol( rhs.m_boundingVol )
   , m_worldSpaceBounds( rhs.m_worldSpaceBounds )
   , m_renderState( new RenderState( *rhs.m_renderState ) )
   , m_shadowCaster( rhs.m_shadowCaster )
   , m_enabled( true )
{
}

///////////////////////////////////////////////////////////////////////////////

GeometryComponent::GeometryComponent( GeometryResource& resource, const char* name )
   : Component( name )
   , m_resource( &resource )
   , m_renderState( new RenderState() )
   , m_shadowCaster( true )
   , m_enabled( true )
{
   m_boundingVol = resource.getBoundingVolume( );
   m_worldSpaceBounds = m_boundingVol;
}

///////////////////////////////////////////////////////////////////////////////

GeometryComponent::~GeometryComponent()
{
   m_resource = NULL;

   // release the render state
   m_renderState->removeReference();
   m_renderState = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::setMaterial( Material* material )
{
   m_renderState->setMaterial( material );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::render( Renderer& renderer, RenderProfileId profile )
{
   if ( !m_resource || !m_enabled || !m_renderState )
   {
      return;
   }

   RCBindShader* shaderComm = NULL;
   switch ( profile )
   {
      case RP_Default:
      {
         shaderComm = m_renderState->renderDefault( renderer );
         break;
      }

      case RP_Selection:
      {
         shaderComm = m_renderState->renderSelection( renderer );
         break;
      }

      case RP_SelectionQuery:
      {
         shaderComm = m_renderState->renderSelectionQuery( renderer );
         break;
      }

      case RP_ShadowMap:
      {
         shaderComm = m_renderState->rendererShadowMap( renderer );
         break;
      }
   }

   if ( shaderComm )
   {
      m_resource->render( renderer );
      m_renderState->postRender( renderer );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::getBoundingBox( AxisAlignedBox& boundingBox ) const
{
   // TODO: when the node is disabled for rendering, invalidate its bounding volume so that it's
   // no longer picked up by the rendering view

   boundingBox = m_boundingVol;
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::setMesh( GeometryResource& mesh )
{
   m_resource = &mesh;
   m_boundingVol = m_resource->getBoundingVolume();
}

///////////////////////////////////////////////////////////////////////////////

std::string GeometryComponent::getMeshName() const
{
   if ( m_resource )
   {
      return m_resource->getFilePath();
   }
   else
   {
      return "<<uninitialized geometry>>";
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::onAttached( Entity* parent )
{
   Component::onAttached( parent );

   parent->addBoundingVolume( &m_boundingVol );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::onDetached( Entity* parent )
{
   parent->removeBoundingVolume( &m_boundingVol );

   Component::onDetached( parent );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::onAttachToModel( Model* model )
{
   Component::onAttachToModel( model );

   m_renderState->onInitialize( this );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::onDetachFromModel( Model* model )
{
   m_renderState->onDeinitialize();

   Component::onDetachFromModel( model );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::onObjectLoaded( )
{
   Component::onModelLoaded();

   // upgrade from the old version - if the render state instance doesn't exist, create it
   if ( !m_renderState )
   {
      m_renderState = new RenderState();
   }

   if ( m_resource )
   {
      m_boundingVol = m_resource->getBoundingVolume();
      m_worldSpaceBounds = m_boundingVol;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GeometryComponent::updateTransforms( )
{
   Component::updateTransforms();

   m_boundingVol.transform( getGlobalMtx( ), m_worldSpaceBounds );
}

///////////////////////////////////////////////////////////////////////////////
