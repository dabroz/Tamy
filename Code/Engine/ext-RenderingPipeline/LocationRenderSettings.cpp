#include "ext-RenderingPipeline\LocationRenderSettings.h"
#include "core-Renderer\CubeTexture.h"
#include "core-Renderer\RenderableCubeTexture.h"

// logging
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( LocationRenderSettings );
   PARENT( Component );
   PROPERTY_EDIT( "Skybox texture", CubeTexture*, m_skybox );
   PROPERTY_EDIT( "Ambient light", Color, m_ambientLight );
   PROPERTY_EDIT( "Ambient occlusion power", float, m_aoPower );
   PROPERTY_EDIT( "Ambient occlusion radius", float, m_aoRadius );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

LocationRenderSettings::LocationRenderSettings( const char* name )
   : Component( name )
   , m_skybox( NULL )
   , m_skyboxTexture( NULL )
   , m_ambientLight( 127, 127, 127 )
   , m_aoPower( 1.0f )
   , m_aoRadius( 0.3f )
{
}

///////////////////////////////////////////////////////////////////////////////

LocationRenderSettings::LocationRenderSettings( const LocationRenderSettings& rhs )
   : Component( rhs )
   , m_skybox( rhs.m_skybox )
   , m_skyboxTexture( NULL )
   , m_ambientLight( rhs.m_ambientLight )
   , m_aoPower( rhs.m_aoPower )
   , m_aoRadius( rhs.m_aoRadius )
{
}

///////////////////////////////////////////////////////////////////////////////

LocationRenderSettings::~LocationRenderSettings()
{
   ASSERT_MSG( m_skyboxTexture == NULL, "LocationRenderSettings: onDetachFromModel wasn't called" );
   m_skybox = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void LocationRenderSettings::onAttachToModel( Model* model )
{
   Component::onAttachToModel( model );
   initializeRuntimeSkybox();
}

///////////////////////////////////////////////////////////////////////////////

void LocationRenderSettings::onDetachFromModel( Model* model )
{
   Component::onDetachFromModel( model );

   deinitializeRuntimeSkybox();
}

///////////////////////////////////////////////////////////////////////////////

void LocationRenderSettings::onPrePropertyChanged( ReflectionProperty& property )
{
   Component::onPrePropertyChanged( property );
   deinitializeRuntimeSkybox();
}

///////////////////////////////////////////////////////////////////////////////

void LocationRenderSettings::onPropertyChanged( ReflectionProperty& property )
{
   Component::onPropertyChanged( property );
   initializeRuntimeSkybox();
}

///////////////////////////////////////////////////////////////////////////////

void LocationRenderSettings::initializeRuntimeSkybox()
{
   if ( m_skybox )
   {
      m_skyboxTexture = new RenderableCubeTexture();
      m_skyboxTexture->setTexture( m_skybox );
   }

}

///////////////////////////////////////////////////////////////////////////////

void LocationRenderSettings::deinitializeRuntimeSkybox()
{
   if ( m_skyboxTexture )
   {
      m_skyboxTexture->removeReference();
      m_skyboxTexture = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void LocationRenderSettings::setSkybox( class CubeTexture* skybox )
{
   NOTIFY_PROPERTY_CHANGE( m_skybox );
   m_skybox = skybox;
}

///////////////////////////////////////////////////////////////////////////////
