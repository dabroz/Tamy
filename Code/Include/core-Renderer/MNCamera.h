/// @file   core-Renderer/MNCamera.h
/// @brief  a node that provides information about the active camera
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSFloatOutput;
class MSMatrixOutput;

///////////////////////////////////////////////////////////////////////////////

class MNCamera : public MaterialNode
{
   DECLARE_ALLOCATOR( MNCamera, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   MSFloatOutput*    m_nearZ;
   MSFloatOutput*    m_farZ;
   MSFloatOutput*    m_viewportWidth;
   MSFloatOutput*    m_viewportHeight;

   uint              var_nearZ;
   uint              var_farZ;
   uint              var_viewportWidth;
   uint              var_viewportHeight;

public:
   /**
    * Constructor.
    */
   MNCamera();

   /**
    * Copy constructor.
    */
   MNCamera( const MNCamera& rhs );
   ~MNCamera();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded() override;

   // -------------------------------------------------------------------------
   // MaterialNode implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
   void render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const override;
};

///////////////////////////////////////////////////////////////////////////////
