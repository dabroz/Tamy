/// @file   ext-RenderingPipeline\MNPhongBlinn.h
/// @brief  a surface shader that prepares data for deferred rendering using the phong-blinn shading model
#pragma once

#include "core-Renderer\MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSFloatInput;
class MSColorOutput;
class MSVec3Input;

///////////////////////////////////////////////////////////////////////////////

/**
* Node that relays forward rendering related data about a directional light to the material system.
*/
class MNPhongBlinn : public MaterialNode
{
   DECLARE_ALLOCATOR( MNPhongBlinn, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // runtime data
   MSFloatInput*                 m_specularIntensity;
   MSFloatInput*                 m_specularHardness;
   MSVec3Input*                  m_normalsVS;

   MSColorOutput*                m_outBRDF;
   MSColorOutput*                m_outGBufNormals;

public:
   /**
   * Constructor.
   *
   * @param name
   */
   MNPhongBlinn( const char* name = "MNPhongBlinn" );

   /**
   * Copy constructor.
   */
   MNPhongBlinn( const MNPhongBlinn& rhs );
   ~MNPhongBlinn();

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
