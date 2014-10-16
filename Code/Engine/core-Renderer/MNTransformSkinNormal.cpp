#include "core-Renderer\MNTransformSkinNormal.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\Shader.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNTransformSkinNormal );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNTransformSkinNormal::MNTransformSkinNormal( const char* name )
   : MaterialNode( name )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_boneTransforms = new MSMatrixArrInput( "Bones" );
      m_blendWeights = new MSVec4Input( "Blend weights" );
      m_blendIndices = new MSVec4Input( "Blend indices" );
      m_transform = new MSMatrixInput( "Transform" );
      m_input = new MSVec3Input( "Input" );

      std::vector< GBNodeInput< MaterialNode >* > inputs;
      inputs.push_back( m_boneTransforms );
      inputs.push_back( m_blendWeights );
      inputs.push_back( m_blendIndices );
      inputs.push_back( m_transform );
      inputs.push_back( m_input );
      defineInputs( inputs );

      m_output = new MSVec3Output( "Output" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNTransformSkinNormal::MNTransformSkinNormal( const MNTransformSkinNormal& rhs )
   : MaterialNode( rhs )
{
   m_boneTransforms = static_cast< MSMatrixArrInput* >( findInput( "Bones" ) );
   m_blendWeights = static_cast< MSVec4Input* >( findInput( "Blend weights" ) );
   m_blendIndices = static_cast< MSVec4Input* >( findInput( "Blend indices" ) );
   m_transform = static_cast< MSMatrixInput* >( findInput( "Transform" ) );
   m_input = static_cast< MSVec3Input* >( findInput( "Input" ) );
   m_output = static_cast< MSVec3Output* >( findOutput( "Output" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNTransformSkinNormal::~MNTransformSkinNormal()
{
}

///////////////////////////////////////////////////////////////////////////////

void MNTransformSkinNormal::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   m_boneTransforms = static_cast< MSMatrixArrInput* >( findInput( "Bones" ) );
   m_blendWeights = static_cast< MSVec4Input* >( findInput( "Blend weights" ) );
   m_blendIndices = static_cast< MSVec4Input* >( findInput( "Blend indices" ) );
   m_transform = static_cast< MSMatrixInput* >( findInput( "Transform" ) );
   m_input = static_cast< MSVec3Input* >( findInput( "Input" ) );
   m_output = static_cast< MSVec3Output* >( findOutput( "Output" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNTransformSkinNormal::buildMaterialShader( MaterialShaderBuilder& builder )
{
   if ( m_boneTransforms->isConnected() && m_blendWeights->isConnected() && m_blendIndices->isConnected() )
   {
      const char* transform = builder.getVariable( m_transform );
      const char* boneTransforms = builder.getVariable( m_boneTransforms );
      const char* blendWeights = builder.getVariable( m_blendWeights );
      const char* blendIndices = builder.getVariable( m_blendIndices );
      const char* inputVec = builder.getVariable( m_input );
      const char* outputVar = builder.addVariable( m_output );

      const char* worldViewMtx = builder.addLocalVariable< MSMatrixArrInput >( "worldViewMtx" );

      const char* suffix[4] = { "x", "y", "z", "w" };
      const char* operators[4] = { "=", "+=", "+=", "+=" };
      const char* indent[4] = { "", "\t", "\t", "\t" };

      // declare variables
      builder.addCodeLine( "// %s implementation", getName().c_str() );
      builder.addCodeLine( "<type_Vector3> %s;", outputVar );

      // create the code
      for ( byte i = 0; i < 4; ++i )
      {
         if ( i > 0 )
         {
            builder.addCodeLine( "if ( %s.%s >= 0 ) {", blendWeights, suffix[i] );
         }

         builder.addCodeLine( "%s%s = (%s * %s[int(%s.%s)]);", indent[i], worldViewMtx, transform, boneTransforms, blendIndices, suffix[i] );
         builder.addCodeLine("%s%s %s %s.%s * (<type_Mtx3x3>( %s ) * %s);", indent[i], outputVar, operators[i], blendWeights, suffix[i], worldViewMtx, inputVec );

         if ( i > 0 )
         {
            builder.addCodeLine( "}\n" );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
