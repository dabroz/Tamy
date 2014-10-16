/// @file   core-Renderer/MNSplitColor.h
/// @brief  splits a color into separate channels
#pragma once

#include "core-Renderer\MaterialNode.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

class MSColorInput;
class MSFloatOutput;

///////////////////////////////////////////////////////////////////////////////

class MNSplitColor : public MaterialNode
{
   DECLARE_ALLOCATOR( MNSplitColor, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // runtime data
   MSColorInput*        m_input;
   MSFloatOutput*       m_outRed;
   MSFloatOutput*       m_outGreen;
   MSFloatOutput*       m_outBlue;
   MSFloatOutput*       m_outAlpha;

public:
   /**
   * Constructor.
   *
   * @param name
   */
   MNSplitColor( const char* name = "MNSplitColor" );

   /**
   * Copy constructor.
   */
   MNSplitColor( const MNSplitColor& rhs );
   ~MNSplitColor();

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
