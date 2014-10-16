/// @file   core-Renderer/RenderingParams.h
/// @brief  texture parameters definitions
#pragma once


///////////////////////////////////////////////////////////////////////////////

enum TextureAddressingMode
{
   TAM_WRAP            = 0,
   TAM_MIRROR          = 1,
   TAM_CLAMP           = 2,
   TAM_BORDER          = 3,
   TAM_MIRRORONCE      = 4,

   TAM_COUNT           = 5,
   TAM_FORCE_BYTE      = 0x7f
};

///////////////////////////////////////////////////////////////////////////////

enum TextureFilteringMode
{
   TFM_NONE                = 0,    // filtering disabled
   TFM_POINT               = 1,    // nearest
   TFM_LINEAR              = 2,    // linear interpolation
   TFM_ANISOTROPIC         = 3,    // anisotropic

   TFM_COUNT               = 3,
   TFM_FORCE_BYTE          = 0x7f
};

///////////////////////////////////////////////////////////////////////////////

enum CullingMode 
{
   CULL_NONE                = 1,
   CULL_CW                  = 2,
   CULL_CCW                 = 3,

   CULL_FORCE_BYTE          = 0x7f
};

///////////////////////////////////////////////////////////////////////////////

enum ComparisonFunc
{
   CMP_NEVER          = 1,
   CMP_LESS           = 2,
   CMP_EQUAL          = 3,
   CMP_LESSEQUAL      = 4,
   CMP_GREATER        = 5,
   CMP_NOTEQUAL       = 6,
   CMP_GREATEREQUAL   = 7,
   CMP_ALWAYS         = 8,

   CMP_FORCE_BYTE     = 0x7f
};

///////////////////////////////////////////////////////////////////////////////

enum BlendFunc
{
   BLEND_ZERO              = 1,
   BLEND_ONE               = 2,
   BLEND_SRCCOLOR          = 3,
   BLEND_INVSRCCOLOR       = 4,
   BLEND_SRCALPHA          = 5,
   BLEND_INVSRCALPHA       = 6,
   BLEND_DESTALPHA         = 7,
   BLEND_INVDESTALPHA      = 8,
   BLEND_DESTCOLOR         = 9,
   BLEND_INVDESTCOLOR      = 10,
   BLEND_SRCALPHASAT       = 11,

   BLEND_FORCE_BYTE       = 0x7f 
};

///////////////////////////////////////////////////////////////////////////////

enum StencilOp
{
   STENCILOP_KEEP          = 1,
   STENCILOP_ZERO          = 2,
   STENCILOP_REPLACE       = 3,
   STENCILOP_INCRSAT       = 4,
   STENCILOP_DECRSAT       = 5,
   STENCILOP_INVERT        = 6,
   STENCILOP_INCR          = 7,
   STENCILOP_DECR          = 8,
   STENCILOP_FORCE_BYTE   = 0x7f
};

///////////////////////////////////////////////////////////////////////////////
