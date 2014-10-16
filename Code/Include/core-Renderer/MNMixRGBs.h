/// @file   core-Renderer/MNMixRGBs.h
/// @brief  mixes two colors together
#pragma once

#include "core-Renderer\MaterialNode.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

class MSColorInput;
class MSColorOutput;

///////////////////////////////////////////////////////////////////////////////

/**
 * CAUTION: the values of this enum need to stay in sync with the mapping
 * performed by the blender exporter - tamy_material.py : getTextureSettings(...)
 */
enum MNRGBOperation
{
   RGBO_Linear_Light,
   RGBO_Soft_Light,
   RGBO_Color,
   RGBO_Saturation,
   RGBO_Hue,
   RGBO_Burn,
   RGBO_Dodge,
   RGBO_Overlay,
   RGBO_Lighten,
   RGBO_Darken,
   RGBO_Difference,
   RGBO_Divide,
   RGBO_Screen,
   RGBO_Subtract,
   RGBO_Multiply,
   RGBO_Add,
   RGBO_Mix
};

///////////////////////////////////////////////////////////////////////////////

class MNMixRGBs : public MaterialNode
{
   DECLARE_ALLOCATOR( MNMixRGBs, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   MNRGBOperation       m_operation;
   float                m_factor;

   // runtime data
   MSColorInput*        m_input1;
   MSColorInput*        m_input2;
   MSColorOutput*       m_output;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param operation
    * @param factor
    */
   MNMixRGBs( const char* name = "MNMixRGBs", MNRGBOperation operation = RGBO_Mix, float factor = 0.5f );

   /**
    * Copy constructor.
    */
   MNMixRGBs( const MNMixRGBs& rhs );
   ~MNMixRGBs();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded() override;

   // -------------------------------------------------------------------------
   // MaterialNode implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
};

///////////////////////////////////////////////////////////////////////////////
