/// @file   core-Renderer/MNTransform.h
/// @brief  transform provider
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSVec4Output;

///////////////////////////////////////////////////////////////////////////////

enum TransformationType
{
   SPACE_Model_to_World,
   SPACE_Model_to_View,
   SPACE_Model_To_Projection,
   SPACE_Model_to_InvTranspWorld,
};

///////////////////////////////////////////////////////////////////////////////

class MNTransform : public MaterialNode
{
   DECLARE_ALLOCATOR( MNTransform, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   TransformationType   m_transformType;
   bool                 m_cameraRelative;

   // sockets
   MSMatrixOutput*      m_output;

   uint                 var_transform;

public:
   /**
   * Constructor.
   *
   * @param transformType
   */
   MNTransform( const char* name = "MNTransform", TransformationType transformType = SPACE_Model_To_Projection );

   /**
   * Copy constructor.
   */
   MNTransform( const MNTransform& rhs );
   ~MNTransform();

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
