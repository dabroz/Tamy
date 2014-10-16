#include "core-Renderer\PointLight.h"
#include "core\Sphere.h"
#include "core-MVC\Entity.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( PointLight );
   PARENT( Light );
   PROPERTY_EDIT( "Color", Color, m_color );
   PROPERTY_EDIT( "strength", float, m_strength );
   PROPERTY_EDIT( "Radius", float, m_radius );
   PROPERTY_EDIT( "Falloff", float, m_falloff );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

PointLight::PointLight( const char* name )
   : Light( LT_Point, name )
   , m_color(1, 1, 1, 1 )
   , m_strength( 1.0f )
   , m_radius( 1.0f )
   , m_falloff( 1.0f )
{
   initializeBounds();
}

///////////////////////////////////////////////////////////////////////////////

PointLight::PointLight( const PointLight& rhs )
   : Light( rhs )
   , m_color( rhs.m_color )
   , m_strength( rhs.m_strength )
   , m_radius( rhs.m_radius )
   , m_falloff( rhs.m_falloff )
{
}

///////////////////////////////////////////////////////////////////////////////

PointLight::~PointLight()
{
}

///////////////////////////////////////////////////////////////////////////////

void PointLight::onPropertyChanged( ReflectionProperty& property )
{
   Light::onPropertyChanged( property );

   if ( property.getName() == "m_radius" )
   {
      initializeBounds();
   }
}

///////////////////////////////////////////////////////////////////////////////

void PointLight::onObjectLoaded()
{
   Light::onObjectLoaded();
   initializeBounds();
}


///////////////////////////////////////////////////////////////////////////////

void PointLight::initializeBounds()
{
   Sphere sphere;
   sphere.set( Quad_0, m_radius );
   sphere.calculateBoundingBox( m_boundingVol );
}

///////////////////////////////////////////////////////////////////////////////
