/// @file   ext-RenderingPipeline\LocationRenderSettings.h
/// @brief  a component that renders as a skybox
#pragma once

#include "core-MVC\Component.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

class LocationRenderSettings : public Component
{
   DECLARE_ALLOCATOR( LocationRenderSettings, AM_DEFAULT );
   DECLARE_CLASS();

public:
   // static data
   Color                         m_ambientLight;

   // ambient occlusion
   float                         m_aoPower;
   float                         m_aoRadius;

private:
   // static data
   class CubeTexture*            m_skybox;

private:
   // runtime data
   class RenderableCubeTexture*  m_skyboxTexture;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   LocationRenderSettings( const char* name = "LocationRenderSettings" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   LocationRenderSettings( const LocationRenderSettings& rhs );
   ~LocationRenderSettings();

   /**
    * Sets a new skybox texture.
    *
    * @param skybox
    */
   void setSkybox( class CubeTexture* skybox );
  
   // -------------------------------------------------------------------------
   // Runtime API
   // -------------------------------------------------------------------------
   /**
    * Returns the skyubox texture.
    */
   class RenderableCubeTexture* getSkyboxTexture() const {
      return m_skyboxTexture;
   }

   /**
    * Returns the value of the base ambient light.
    */
   inline const Color& getBaseAmbientLight() const {
      return m_ambientLight;
   }

   // -------------------------------------------------------------------------
   // SceneNode implementation
   // -------------------------------------------------------------------------
   void onAttachToModel( Model* model ) override;
   void onDetachFromModel( Model* model ) override;

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onPrePropertyChanged( ReflectionProperty& property ) override;
   void onPropertyChanged( ReflectionProperty& property ) override;

private:
   void initializeRuntimeSkybox();
   void deinitializeRuntimeSkybox();
};

///////////////////////////////////////////////////////////////////////////////
