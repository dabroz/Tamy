#include "ext-RenderingPipeline\MNCookTorrance.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialEntryStruct.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderingContext.h"
#include "core-Renderer\Shader.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"



///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNCookTorrance );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNCookTorrance::MNCookTorrance( const char* name )
   : MaterialNode( name )
   , m_roughness( NULL )
   , m_reflectivity( NULL )
   , m_normalsVS( NULL )
   , m_outBRDF( NULL )
   , m_outGBufNormals( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      // inputs
      m_roughness = new MSFloatInput( "Roughness" );
      m_reflectivity = new MSFloatInput( "Reflectivity" );
      m_normalsVS = new MSVec3Input( "NormalsVS" );

      std::vector< GBNodeInput< MaterialNode >* > inputs;
      inputs.push_back( m_roughness );
      inputs.push_back( m_reflectivity );
      inputs.push_back( m_normalsVS );
      defineInputs( inputs );

      // outputs
      m_outBRDF = new MSColorOutput( "GBuf_BRDF" );
      m_outGBufNormals = new MSColorOutput( "GBuf_Normals" );

      std::vector< GBNodeOutput< MaterialNode >* > outputs;
      outputs.push_back( m_outBRDF );
      outputs.push_back( m_outGBufNormals );
      defineOutputs( outputs );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNCookTorrance::MNCookTorrance( const MNCookTorrance& rhs )
   : MaterialNode( rhs )
   , m_roughness( NULL )
   , m_reflectivity( NULL )
   , m_normalsVS( NULL )
   , m_outBRDF( NULL )
   , m_outGBufNormals( NULL )
{
   m_roughness = static_cast< MSFloatInput* >( findInput( "Roughness" ) );
   m_reflectivity = static_cast< MSFloatInput* >( findInput( "Reflectivity" ) );
   m_normalsVS = static_cast< MSVec3Input* >( findInput( "NormalsVS" ) );

   m_outBRDF = static_cast< MSColorOutput* >( findOutput( "GBuf_BRDF" ) );
   m_outGBufNormals = static_cast< MSColorOutput* >( findOutput( "GBuf_Normals" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNCookTorrance::~MNCookTorrance()
{
   m_roughness = NULL;
   m_reflectivity = NULL;
   m_normalsVS = NULL;
   m_outBRDF = NULL;
   m_outGBufNormals = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNCookTorrance::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing inputs
   m_roughness = static_cast< MSFloatInput* >( findInput( "Roughness" ) );
   m_reflectivity = static_cast< MSFloatInput* >( findInput( "Reflectivity" ) );
   m_normalsVS = static_cast< MSVec3Input* >( findInput( "NormalsVS" ) );

   m_outBRDF = static_cast< MSColorOutput* >( findOutput( "GBuf_BRDF" ) );
   m_outGBufNormals = static_cast< MSColorOutput* >( findOutput( "GBuf_Normals" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNCookTorrance::buildMaterialShader( MaterialShaderBuilder& builder )
{
   // BRDF definition
   if ( m_outBRDF && m_roughness && m_reflectivity )
   {
      const char* gBufBRDFVar = builder.addVariable( m_outBRDF );

      builder.addCodeLine( "<type_Color> %s = <type_Color>(0);", gBufBRDFVar );
      builder.addCodeLine( "%s.x = 0.2;\t\t // COOK_TORRANCE_SHADING", gBufBRDFVar );

      if ( m_roughness->isConnected() )
      {
         const char* roughness = builder.getVariable( m_roughness );
         builder.addCodeLine( "%s.y = %s;", gBufBRDFVar, roughness );
      }
      else
      {
         builder.addCodeLine( "%s.y = 0.2;", gBufBRDFVar );
      }

      if ( m_reflectivity->isConnected() )
      {
         const char* reflectivity = builder.getVariable( m_reflectivity );
         builder.addCodeLine( "%s.z = %s;", gBufBRDFVar, reflectivity );
      }
      else
      {
         builder.addCodeLine( "%s.z = 0.3;", gBufBRDFVar );
      }
   }

   // normals conversion
   if ( m_outGBufNormals && m_normalsVS && m_normalsVS->isConnected() )
   {
      const char* normalsVS = builder.getVariable( m_normalsVS );
      const char* gBufNormalsVar = builder.addVariable( m_outGBufNormals );

      builder.addCodeLine( "<type_Color> %s = <type_Color>( ( %s + 1.0 ) * 0.5, 1.0 );", gBufNormalsVar, normalsVS );
   }
}

///////////////////////////////////////////////////////////////////////////////
