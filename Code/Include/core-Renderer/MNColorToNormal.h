/// @file   core-Renderer/MNColorToNormal.h
/// @brief  transforms a color value to a normal vector
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSColorInput;
class MSVec3Output;

///////////////////////////////////////////////////////////////////////////////

class MNColorToNormal : public MaterialNode
{
   DECLARE_ALLOCATOR( MNColorToNormal, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // sockets
   MSColorInput*           m_inColor;
   MSVec3Output*           m_outNormal;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param strength
    */
   MNColorToNormal( const char* name = "MNColorToNormal" );

   /**
   * Copy constructor.
   */
   MNColorToNormal( const MNColorToNormal& rhs );
   ~MNColorToNormal();

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
