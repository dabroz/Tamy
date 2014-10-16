/// @file   core-Renderer/MNNormalTangentToView.h
/// @brief  this node provides a texture based normal map
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSVec3Input;
class MSVec3Output;

///////////////////////////////////////////////////////////////////////////////

class MNNormalTangentToView : public MaterialNode
{
   DECLARE_ALLOCATOR( MNNormalTangentToView, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   float                m_strenght;

   // sockets
   MSVec3Input*         m_normalTangentSpace;
   MSVec3Input*         m_vertexTangent;
   MSVec3Input*         m_vertexNormal;
   MSVec3Output*        m_outNormal;

   uint                 var_strength;


public:
   /**
    * Constructor.
    *
    * @param name
    * @param strength
    */
   MNNormalTangentToView( const char* name = "MNNormalTangentToView", float strenght = 1.0f );

   /**
    * Copy constructor.
    */
   MNNormalTangentToView( const MNNormalTangentToView& rhs );
   ~MNNormalTangentToView();

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
