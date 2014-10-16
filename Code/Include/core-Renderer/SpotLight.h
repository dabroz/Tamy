/// @file   core-Renderer/SpotLight.h
/// @brief  spot light component
#pragma once

#include "core-Renderer\Light.h"
#include "core-Renderer\RenderCommand.h"
#include "core\Color.h"
#include "core\AxisAlignedBox.h"
#include "core\FastFloat.h"


///////////////////////////////////////////////////////////////////////////////

/**
* This is a special kind of node that represents a point light in a scene
*/
class SpotLight : public Light
{
   DECLARE_ALLOCATOR( SpotLight, AM_ALIGNED_16 );
   DECLARE_CLASS();

public:
   // static data
   Color                      m_color;
   float                      m_strength;
   float                      m_range;
   float                      m_falloff;
   float                      m_spotAngle;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   SpotLight( const char* name = "SpotLight" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   SpotLight( const SpotLight& rhs );
   virtual ~SpotLight();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged( ReflectionProperty& property ) override;
   void onObjectLoaded() override;

private:
   void initializeBounds();
};

///////////////////////////////////////////////////////////////////////////////
