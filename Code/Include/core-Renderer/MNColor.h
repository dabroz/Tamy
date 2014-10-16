/// @file   core-Renderer/MNColor.h
/// @brief  color source node
#pragma once

#include "core-Renderer\MaterialNode.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

class MSColorOutput;

///////////////////////////////////////////////////////////////////////////////

class MNColor : public MaterialNode
{
   DECLARE_ALLOCATOR( MNColor, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   Color                m_color;

   // runtime data
   MSColorOutput*       m_output;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param color
    */
   MNColor( const char* name = "MNColor", const Color& color = Color( 127, 127, 127 ) );

   /**
    * Copy constructor.
    */
   MNColor( const MNColor& rhs );
   ~MNColor();

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
