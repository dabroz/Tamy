#include "core-Renderer\TextureSamplerSettings.h"
#include "core\InStream.h"
#include "core\OutStream.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

DEFINE_DEFAULT_APPROXIMATE_EQUALITY( TextureSamplerSettings );

///////////////////////////////////////////////////////////////////////////////

void TextureSamplerSettings::setUnified( TextureAddressingMode addressingMode, TextureFilteringMode filteringMode )
{
   m_addressU = addressingMode;
   m_addressV = addressingMode;
   m_addressW = addressingMode;

   m_minFilter = filteringMode;
   m_magFilter = filteringMode;
   m_mipFilter = filteringMode;
}

///////////////////////////////////////////////////////////////////////////////

bool TextureSamplerSettings::operator== ( const TextureSamplerSettings& rhs ) const
{
   // every field can be represented using a byte of data, so let's cast the structure
   // to a set of numbers and subtract those values. If we receive a 0, the structures hold identical data.

   // We have 6 bytes, and the higher supported integer type size on most platforms can hold up to 4 bytes,
   // so we need to use a 4-byte and a 2-byte type ( an unsigned int and an unsigned short ) to get the entire number right
   uint v11 = *( ( uint* ) &m_addressU );
   word v12 = *( ( word* ) &m_magFilter );
   uint v21 = *( ( uint* ) &rhs.m_addressU );
   word v22 = *( ( word* ) &rhs.m_magFilter );

   return ( v11 - v21 ) - ( v12 - v22 ) == 0;
}

///////////////////////////////////////////////////////////////////////////////

bool TextureSamplerSettings::operator!= ( const TextureSamplerSettings& rhs ) const
{
   return !( *this == rhs );
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const TextureSamplerSettings& params )
{
   stream << params.m_addressU;
   stream << params.m_addressV;
   stream << params.m_addressW;
   stream << params.m_minFilter;
   stream << params.m_magFilter;
   stream << params.m_mipFilter;
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, TextureSamplerSettings& params )
{
   stream >> params.m_addressU;
   stream >> params.m_addressV;
   stream >> params.m_addressW;
   stream >> params.m_minFilter;
   stream >> params.m_magFilter;
   stream >> params.m_mipFilter;
   return stream;
}

///////////////////////////////////////////////////////////////////////////////
