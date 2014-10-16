/// @file   core-Renderer\DirectionalLight.h
/// @brief  a directional light component
#ifndef _DIRECTIONAL_LIGHT_H
#define _DIRECTIONAL_LIGHT_H

#include "core-Renderer\Light.h"
#include "core\Color.h"
#include "core\AxisAlignedBox.h"


///////////////////////////////////////////////////////////////////////////////

class GeometryComponent;
class RenderTarget;
class Camera;

///////////////////////////////////////////////////////////////////////////////

/**
 * This is a special kind of node that represents a directional light in a scene
 */
class DirectionalLight : public Light
{
   DECLARE_ALLOCATOR( DirectionalLight, AM_ALIGNED_16 );
   DECLARE_CLASS();

public:
   // static data
   Color                m_color;
   float                m_strength;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   DirectionalLight( const char* name = "DirectionalLight" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   DirectionalLight( const DirectionalLight& rhs );
   ~DirectionalLight();
};

///////////////////////////////////////////////////////////////////////////////

#endif // _DIRECTIONAL_LIGHT_H
