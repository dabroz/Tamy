/// @file   core-Renderer\TextureSamplerSettings.h
/// @brief  description how to set a texture sampler
#pragma once

#include "core\MemoryRouter.h"
#include "core-Renderer\RenderingParams.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class OutStream;
class InStream;

///////////////////////////////////////////////////////////////////////////////

/**
* Texture mapping params
*/
struct TextureSamplerSettings
{
   DECLARE_ALLOCATOR( TextureSamplerSettings, AM_DEFAULT );

   byte           m_addressU;
   byte           m_addressV;
   byte           m_addressW;
   byte           m_minFilter;
   byte           m_magFilter;
   byte           m_mipFilter;

   TextureSamplerSettings()
      : m_addressU( TAM_WRAP )
      , m_addressV( TAM_WRAP )
      , m_addressW( TAM_WRAP )
      , m_minFilter( TFM_LINEAR )
      , m_magFilter( TFM_LINEAR )
      , m_mipFilter( TFM_LINEAR )
   {}

   /**
    * Assigns the same settings across all modes.
    *
    * @param addressingMode
    * @param filteringMode
    */
   void setUnified( TextureAddressingMode addressingMode, TextureFilteringMode filteringMode );

   /**
    * Equality comparator.
    *
    * @param rhs
    */
   bool operator==( const TextureSamplerSettings& rhs ) const;

   /**
    * Inequality comparator.
    *
    * @param rhs
    */
   bool operator!=( const TextureSamplerSettings& rhs ) const;

   // -------------------------------------------------------------------------
   // Serialization support
   // -------------------------------------------------------------------------
   friend OutStream& operator<<( OutStream& stream, const TextureSamplerSettings& params );
   friend InStream& operator>>( InStream& stream, TextureSamplerSettings& params );
};

///////////////////////////////////////////////////////////////////////////////
