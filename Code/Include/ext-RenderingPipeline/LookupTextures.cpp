#include "ext-RenderingPipeline\LookupTextures.h"
#include "core-Renderer\ProceduralTexture.h"
#include "core\types.h"

///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( LookupTextures );

///////////////////////////////////////////////////////////////////////////////

LookupTextures::LookupTextures( const SingletonConstruct& )
{
   initCookTorranceRoughness();
}

///////////////////////////////////////////////////////////////////////////////

LookupTextures::~LookupTextures()
{
   m_cookTorranceRoughness->removeReference();
   m_cookTorranceRoughness = NULL;
}

///////////////////////////////////////////////////////////////////////////////

ProceduralTexture* LookupTextures::getCookTorrenceRoughness() const
{
   m_cookTorranceRoughness->addReference();
   return m_cookTorranceRoughness;
}

///////////////////////////////////////////////////////////////////////////////

void LookupTextures::initCookTorranceRoughness()
{
   const uint lookupDimensions = 512;
   m_cookTorranceRoughness = new ProceduralTexture( lookupDimensions, lookupDimensions, PTF_Float );
   PTFloat* fLookup = ( PTFloat* ) m_cookTorranceRoughness->accessImageBuffer();

   uint addr = 0;
   for ( uint y = 0; y < lookupDimensions; ++y )
   {
      const float roughness = ( float ) y / ( float ) lookupDimensions;

      for ( uint x = 0; x < lookupDimensions; ++x, ++addr )
      {      
         float NdotH = ( float ) x / ( float ) lookupDimensions;
         
         // Convert the 0.0..1.0 ranges to be -1.0..+1.0
         NdotH *= 2.0f;
         NdotH -= 1.0f;

         // Evaluate a Beckmann distribution for this element
         // of the look-up table:
         float r_sq = roughness * roughness;
         float r_a = 1.0f / ( 4.0f * r_sq * pow( NdotH, 4 ) );
         float r_b = NdotH * NdotH - 1.0f;
         float r_c = r_sq * NdotH * NdotH;

         float roughnessVal = 0.0f;
         if ( r_c != 0.0 )
         {
            roughnessVal = r_a * expf( r_b / r_c );
         }
         fLookup[addr].fromFloat( roughnessVal );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
