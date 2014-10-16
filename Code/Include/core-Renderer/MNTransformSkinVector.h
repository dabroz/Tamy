/// @file   core-Renderer\MNTransformSkinVector.h
/// @brief  a transform that takes skinning into account and transforms a position using skinning transforms
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSVec4Input;
class MSVec4Output;
class MSMatrixArrInput;
class MSMatrixInput;

///////////////////////////////////////////////////////////////////////////////

class MNTransformSkinVector : public MaterialNode
{
   DECLARE_ALLOCATOR( MNTransformSkinVector, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // sockets
   MSMatrixArrInput*       m_boneTransforms;
   MSVec4Input*            m_blendWeights;
   MSVec4Input*            m_blendIndices;
   MSMatrixInput*          m_transform;
   MSVec4Input*            m_input;
   MSVec4Output*           m_output;

public:
   /**
   * Constructor.
   *
   * @param name
   */
   MNTransformSkinVector( const char* name = "MNTransformSkinVector" );

   /**
   * Copy constructor.
   */
   MNTransformSkinVector( const MNTransformSkinVector& rhs );
   ~MNTransformSkinVector();

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
