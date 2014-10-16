#include "core-Renderer\MNTransform.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"



///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( TransformationType );
   ENUM_VAL( SPACE_Model_to_World );
   ENUM_VAL( SPACE_Model_to_View );
   ENUM_VAL( SPACE_Model_To_Projection );
   ENUM_VAL( SPACE_Model_to_InvTranspWorld );
END_ENUM();

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNTransform );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "Type", TransformationType, m_transformType );
   PROPERTY_EDIT( "Camera relative", bool, m_cameraRelative );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNTransform::MNTransform( const char* name, TransformationType transformType )
   : MaterialNode( name )
   , m_transformType( transformType )
   , m_cameraRelative( false )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_output = new MSMatrixOutput( "Transform" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNTransform::MNTransform( const MNTransform& rhs )
   : MaterialNode( rhs )
   , m_transformType( rhs.m_transformType )
   , m_cameraRelative( rhs.m_cameraRelative )
{
   m_output = static_cast< MSMatrixOutput* >( findOutput( "Transform" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNTransform::~MNTransform()
{
}

///////////////////////////////////////////////////////////////////////////////

void MNTransform::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   m_output = static_cast< MSMatrixOutput* >( findOutput( "Transform" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNTransform::buildMaterialShader( MaterialShaderBuilder& builder )
{
   var_transform = builder.addGlobal( m_output );
}

///////////////////////////////////////////////////////////////////////////////

void MNTransform::render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const
{
   const GeometryComponent* parent = host.getParent();
   if ( !parent )
   {
      return;
   }

   Matrix transformMtx;
   Matrix renderedNodeWorldMtx;
   Camera& camera = renderer.getActiveCamera();

   if ( m_cameraRelative )
   {
      const Matrix& nodeMtx = host.getParent()->getGlobalMtx();

      Vector cameraPos;
      camera.getPosition( cameraPos );
      Vector dirToCamera;
      dirToCamera.setSub( cameraPos, nodeMtx.position() );
      const FastFloat scale = dirToCamera.length();

      Matrix scaleMtx;
      scaleMtx.scaleUniform( scale );
      renderedNodeWorldMtx.setMul( scaleMtx, nodeMtx );
   }
   else
   {
      renderedNodeWorldMtx = host.getParent()->getGlobalMtx();
   }

   switch ( m_transformType )
   {
      case SPACE_Model_to_World:
      {
         transformMtx = renderedNodeWorldMtx;
         break;
      }

      case SPACE_Model_to_View:
      {
         Camera& camera = renderer.getActiveCamera();
         transformMtx.setMul( renderedNodeWorldMtx, camera.getViewMtx() );
         break;
      }

      case SPACE_Model_To_Projection:
      {
         Camera& camera = renderer.getActiveCamera();

         Matrix worldViewMtx;
         worldViewMtx.setMul( renderedNodeWorldMtx, camera.getViewMtx() );

         transformMtx.setMul( worldViewMtx, camera.getProjectionMtx() );
         break;
      }

      case SPACE_Model_to_InvTranspWorld:
      {
         transformMtx.setInverseTranspose( renderedNodeWorldMtx );
         break;
      }
   }

   constantsBuf->accessData< Matrix >( var_transform ) = transformMtx;
}

///////////////////////////////////////////////////////////////////////////////
