#include "core-Renderer\DirectionalLight.h"
#include "core-MVC\Entity.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( DirectionalLight );
   PARENT( Light );
   PROPERTY_EDIT( "Color", Color, m_color );
   PROPERTY_EDIT( "strength", float, m_strength );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

DirectionalLight::DirectionalLight( const char* name )
   : Light( LT_Directional, name )
   , m_color(1, 1, 1, 1 )
   , m_strength( 1 )
{
   m_boundingVol.set( Vector( -100000.0f, -100000.0f, -100000.0f ), Vector( 100000.0f, 100000.0f, 100000.0f ) );
}

///////////////////////////////////////////////////////////////////////////////

DirectionalLight::DirectionalLight( const DirectionalLight& rhs )
   : Light( rhs )
   , m_color( rhs.m_color )
   , m_strength( rhs.m_strength )
{
}

///////////////////////////////////////////////////////////////////////////////

DirectionalLight::~DirectionalLight()
{
}

///////////////////////////////////////////////////////////////////////////////
