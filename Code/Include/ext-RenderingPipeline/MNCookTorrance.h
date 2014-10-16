/// @file   ext-RenderingPipeline\MNCookTorrance.h
/// @brief  a surface shader that uses the Cook-Torrance shading model
#pragma once

#include "core-Renderer\MaterialNode.h"
#include "core-Renderer\TextureSamplerSettings.h"


///////////////////////////////////////////////////////////////////////////////

class MSVec4Input;
class MSVec3Input;
class MSFloatInput;
class MSColorInput;
class MSColorOutput;

///////////////////////////////////////////////////////////////////////////////

/**
* Node that relays forward rendering related data about a directional light to the material system.
*/
class MNCookTorrance : public MaterialNode
{
   DECLARE_ALLOCATOR( MNCookTorrance, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // runtime data
   MSFloatInput*                 m_roughness;
   MSFloatInput*                 m_reflectivity;
   MSVec3Input*                  m_normalsVS;

   MSColorOutput*                m_outBRDF;
   MSColorOutput*                m_outGBufNormals;

public:
   /**
   * Constructor.
   *
   * @param name
   */
   MNCookTorrance( const char* name = "MNCookTorrance" );

   /**
   * Copy constructor.
   */
   MNCookTorrance( const MNCookTorrance& rhs );
   ~MNCookTorrance();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded() override;

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
};

///////////////////////////////////////////////////////////////////////////////
