#include "ext-RenderingPipeline\MNBoneMatrices.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\ShaderDataBuffer.h"

// skeleton
#include "core-AI\SkeletonComponent.h"
#include "core-AI\Skeleton.h"

// MVC
#include "core-MVC\EntityUtils.h"
#include "core-MVC\Entity.h"

// utils
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNBoneMatrices );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNBoneMatrices::MNBoneMatrices( const char* name )
   : MaterialNode( name )
   , m_matricesPtr( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_matricesPtr = new MSMatrixArrOutput( "Bones" );
      defineOutput( m_matricesPtr );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNBoneMatrices::MNBoneMatrices( const MNBoneMatrices& rhs )
   : MaterialNode( rhs )
   , m_matricesPtr( NULL )
{
   m_matricesPtr = static_cast< MSMatrixArrOutput* >( findOutput( "Bones" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNBoneMatrices::~MNBoneMatrices()
{
   m_matricesPtr = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNBoneMatrices::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing inputs
   m_matricesPtr = static_cast< MSMatrixArrOutput* >( findOutput( "Bones" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNBoneMatrices::buildMaterialShader( MaterialShaderBuilder& builder )
{
   var_matricesPtr = builder.addGlobal( m_matricesPtr );
}

///////////////////////////////////////////////////////////////////////////////

void MNBoneMatrices::onCreateLayout( const RenderState& host ) const
{
   RuntimeDataBuffer& data = host.data();
   data.registerVar( m_skeletonComp );
   data.registerVar( m_skinningMtx );

   const GeometryComponent* geomComp = host.getParent();
   if ( geomComp )
   {
      const Entity* entity = geomComp->getParent();

      const SkeletonComponent* skeletonComp = EntityUtils::getFirstChild< SkeletonComponent >( entity );
      data[m_skeletonComp] = skeletonComp;

      if ( skeletonComp && skeletonComp->m_skeleton )
      {
         uint bonesCount = skeletonComp->m_skeleton->getBoneCount();
         Array< Matrix >& skinningMtx = data[m_skinningMtx];
         skinningMtx.resizeWithoutInitializing( bonesCount );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void MNBoneMatrices::onDestroyLayout( const RenderState& host ) const
{
   RuntimeDataBuffer& data = host.data();
   data.unregisterVar( m_skeletonComp );

   Array< Matrix >& skinningMtx = data[m_skinningMtx];
   skinningMtx.clear();
   data.unregisterVar( m_skinningMtx );
}

///////////////////////////////////////////////////////////////////////////////

void MNBoneMatrices::render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const
{
   RuntimeDataBuffer& data = host.data();
   const SkeletonComponent* skeletonComp = data[m_skeletonComp];

   if ( skeletonComp && skeletonComp->m_skeleton )
   {
      const uint boneCount = skeletonComp->m_skeleton->getBoneCount();
      Matrix* matricesPtr = &constantsBuf->accessData< Matrix >( var_matricesPtr );
      memcpy( matricesPtr, skeletonComp->m_skinningMtx, boneCount * sizeof( Matrix ) );
   }
}

///////////////////////////////////////////////////////////////////////////////
