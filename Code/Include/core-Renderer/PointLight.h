/// @file   core-Renderer/PointLight.h
/// @brief  point light component
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
class PointLight : public Light
{
   DECLARE_ALLOCATOR( PointLight, AM_ALIGNED_16 );
   DECLARE_CLASS();

public:
   // static data
   Color                      m_color;
   float                      m_strength;
   float                      m_radius;
   float                      m_falloff;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   PointLight( const char* name = "PointLight" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   PointLight( const PointLight& rhs );
   virtual ~PointLight();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged( ReflectionProperty& property ) override;
   void onObjectLoaded() override;

private:
   void initializeBounds();
};

///////////////////////////////////////////////////////////////////////////////
