#include "core-Renderer/Light.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( Light );
   PARENT( Component );
   PROPERTY_EDIT( "Casts shadows", bool, m_castsShadows );
   PROPERTY_EDIT( "Enabled", bool, m_enabled );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

Light::Light( LightType type, const char* name )
   : Component( name )
   , m_lightType( type )
   , m_castsShadows( false )
   , m_enabled( true )
{
}

///////////////////////////////////////////////////////////////////////////////

Light::Light( const Light& rhs )
   : Component( rhs )
   , m_lightType( rhs.m_lightType )
   , m_castsShadows( rhs.m_castsShadows )
   , m_enabled( rhs.m_enabled )
   , m_boundingVol( rhs.m_boundingVol )
   , m_worldSpaceBounds( rhs.m_worldSpaceBounds )
{
}

///////////////////////////////////////////////////////////////////////////////

void Light::getBoundingBox( AxisAlignedBox& boundingBox ) const
{
   // TODO: when the node is disabled for rendering, invalidate its bounding volume so that it's
   // no longer picked up by the rendering view

   boundingBox = m_boundingVol;
}

///////////////////////////////////////////////////////////////////////////////

void Light::updateTransforms()
{
   Component::updateTransforms();

   m_boundingVol.transform( getGlobalMtx(), m_worldSpaceBounds );
}

///////////////////////////////////////////////////////////////////////////////
