/// @file   ext-RenderingPipeline\LookupTextures.h
/// @brief  a collection of lookup textures used by various algorithms
#pragma once

#include "core\MemoryRouter.h"
#include "core\Singleton.h"


///////////////////////////////////////////////////////////////////////////////

class LookupTextures
{
   REGULAR_SINGLETON();
   DECLARE_ALLOCATOR( LookupTextures, AM_DEFAULT );

private:
   class ProceduralTexture*            m_cookTorranceRoughness;

public:
   /**
    * A singleton constructor.
    */
   LookupTextures( const SingletonConstruct& );
   ~LookupTextures();

   /**
    * Returns a roughness lookup texture used by the Cook-Torrance surface shader.
    */
   class ProceduralTexture* getCookTorrenceRoughness() const;

private:
   void initCookTorranceRoughness();
};

///////////////////////////////////////////////////////////////////////////////
