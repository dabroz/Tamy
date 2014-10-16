/// @file   core-Renderer\MNTransformSkinNormal.h
/// @brief  a transform that takes skinning into account and transforms a normal vector using skinning transforms
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSVec3Input;
class MSVec4Input;
class MSVec3Output;
class MSMatrixArrInput;
class MSMatrixInput;

///////////////////////////////////////////////////////////////////////////////

class MNTransformSkinNormal : public MaterialNode
{
   DECLARE_ALLOCATOR( MNTransformSkinNormal, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // sockets
   MSMatrixArrInput*       m_boneTransforms;
   MSVec4Input*            m_blendWeights;
   MSVec4Input*            m_blendIndices;
   MSMatrixInput*          m_transform;
   MSVec3Input*            m_input;
   MSVec3Output*           m_output;

public:
   /**
   * Constructor.
   *
   * @param name
   */
   MNTransformSkinNormal( const char* name = "MNTransformSkinNormal" );

   /**
   * Copy constructor.
   */
   MNTransformSkinNormal( const MNTransformSkinNormal& rhs );
   ~MNTransformSkinNormal();

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
