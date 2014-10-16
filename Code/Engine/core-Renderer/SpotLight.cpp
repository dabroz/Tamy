#include "core-Renderer\SpotLight.h"
#include "core\Sphere.h"
#include "core-MVC\Entity.h"
#include "core\Math.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SpotLight );
   PARENT( Light );
   PROPERTY_EDIT( "Color", Color, m_color );
   PROPERTY_EDIT( "strength", float, m_strength );
   PROPERTY_EDIT( "Range", float, m_range );
   PROPERTY_EDIT( "Falloff", float, m_falloff );
   PROPERTY_EDIT( "Spot angle", float, m_spotAngle );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

SpotLight::SpotLight( const char* name )
   : Light( LT_Spot, name )
   , m_color( 1, 1, 1, 1 )
   , m_strength( 1.0f )
   , m_range( 10.0f )
   , m_falloff( 1.0f )
   , m_spotAngle( 30.0f )
{
   initializeBounds();
}

///////////////////////////////////////////////////////////////////////////////

SpotLight::SpotLight( const SpotLight& rhs )
   : Light( rhs )
   , m_color( rhs.m_color )
   , m_strength( rhs.m_strength )
   , m_range( rhs.m_range )
   , m_falloff( rhs.m_falloff )
   , m_spotAngle( rhs.m_spotAngle )
{
}

///////////////////////////////////////////////////////////////////////////////

SpotLight::~SpotLight()
{
}

///////////////////////////////////////////////////////////////////////////////

void SpotLight::onPropertyChanged( ReflectionProperty& property )
{
   Light::onPropertyChanged( property );

   if ( property.getName() == "m_range" )
   {
      // normalize the values so that they don't exceed specific values
      m_range = max2<float>( m_range, 0.01f );

      initializeBounds();
   }

   if ( property.getName() == "m_spotAngle" )
   {
      // normalize the values so that they don't exceed specific values
      m_spotAngle = clamp<float>( m_spotAngle, 1.0f, 89.0f );

      initializeBounds();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SpotLight::onObjectLoaded()
{
   Light::onObjectLoaded();
   initializeBounds();
}

///////////////////////////////////////////////////////////////////////////////

void SpotLight::initializeBounds()
{
   // calculate the radius of a circumcircle of a triangle created by
   // cutting the spotlight shape in half along its range vector
   float radius = 0.0f;
   {
      // the triangle has two equal-length edges and a base
      const float halfBaseLength = tan( DEG2RAD( m_spotAngle * 0.5f ) ) * m_range;
      const float baseLength = 2.0f * halfBaseLength;
      const float edgeLength = sqrt( halfBaseLength*halfBaseLength + m_range*m_range );

      const float nominator = edgeLength*edgeLength*baseLength;
      const float denominator = ( 2.0f * edgeLength + baseLength ) * ( baseLength * baseLength ) * ( 2.0f * edgeLength - baseLength );
      radius = nominator / sqrt( denominator );
   }

   // the sphere is centered about a point halfway along the shine vector, which is pointing
   // along the positive Z axis and is m_range long
   Vector sphereOrigin;
   sphereOrigin.setMul( Vector_OZ, FastFloat::fromFloat( m_range * 0.5f ) );

   Sphere sphere( sphereOrigin, radius );
   sphere.calculateBoundingBox( m_boundingVol );
}

///////////////////////////////////////////////////////////////////////////////
