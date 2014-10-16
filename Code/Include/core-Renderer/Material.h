/// @file   core-Renderer\Material.h
/// @brief  material resource
#pragma once

#include "core\Resource.h"


///////////////////////////////////////////////////////////////////////////////

class MaterialProfile;

///////////////////////////////////////////////////////////////////////////////

class Material : public Resource
{
   DECLARE_ALLOCATOR( Material, AM_DEFAULT );
   DECLARE_RESOURCE()

private:
   // static data
   MaterialProfile*                    m_profile;

public:
   /**
    * Constructor.
    *
    * @param resourceName
    */
   Material( const FilePath& resourceName = FilePath() );
   ~Material();

   /**
    * Builds the material.
    */
   void buildMaterial();

   /**
    * Clears the material.
    */
   void clear();

   /**
    * Returns the profile.
    */
   inline const MaterialProfile* getProfile() const {
      return m_profile;
   }

   /**
   * Returns the profile (non-const).
   */
   inline MaterialProfile* accessProfile() {
      return m_profile;
   }

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
   void onResourceLoaded( ResourcesManager& mgr );
};

///////////////////////////////////////////////////////////////////////////////
