#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialProfile.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( Material, tmat, AM_BINARY )
   PROPERTY( MaterialProfile*, m_profile );
END_RESOURCE()

///////////////////////////////////////////////////////////////////////////////

Material::Material( const FilePath& resourceName )
   : Resource( resourceName )
   , m_profile( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

Material::~Material()
{
   if ( m_profile )
   {
      m_profile->removeReference();
      m_profile = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Material::replaceContents( Resource& rhs )
{
   Material& rhsMaterial = static_cast< Material& >( rhs );

   m_profile = rhsMaterial.m_profile;
   if ( m_profile )
   {
      m_profile->addReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Material::onResourceLoaded( ResourcesManager& mgr )
{
   Resource::onResourceLoaded( mgr );

   if ( !m_profile )
   {
      m_profile = new MaterialProfile();
   }

   m_profile->onResourceLoaded( mgr, getFilePath() );
}

///////////////////////////////////////////////////////////////////////////////

void Material::buildMaterial()
{
   m_profile->buildProfile();
}

///////////////////////////////////////////////////////////////////////////////

void Material::clear()
{
   m_profile->clear();
}

///////////////////////////////////////////////////////////////////////////////
