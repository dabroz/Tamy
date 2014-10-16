/// @file   ext-RenderingPipeline\MNBoneMatrices.h
/// @brief  supplies bone matrices
#pragma once

#include "core-Renderer\MaterialNode.h"
#include "core\Array.h"
#include "core\Matrix.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class MSMatrixArrOutput;
class SkeletonComponent;

///////////////////////////////////////////////////////////////////////////////

class MNBoneMatrices : public MaterialNode
{
   DECLARE_ALLOCATOR( MNBoneMatrices, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // runtime data
   MSMatrixArrOutput*                              m_matricesPtr;
   uint                                            var_matricesPtr;

   TRuntimeVar< const SkeletonComponent* >         m_skeletonComp;
   TRuntimeVar< Array< Matrix > >                  m_skinningMtx;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   MNBoneMatrices( const char* name = "MNBoneMatrices" );

   /**
    * Copy constructor.
    */
   MNBoneMatrices( const MNBoneMatrices& rhs );
   ~MNBoneMatrices();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded() override;

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
   void onCreateLayout( const RenderState& host ) const override;
   void onDestroyLayout( const RenderState& host ) const override;
   void render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const override;

};

///////////////////////////////////////////////////////////////////////////////
