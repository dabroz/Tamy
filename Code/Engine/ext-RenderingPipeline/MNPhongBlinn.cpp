#include "ext-RenderingPipeline\MNPhongBlinn.h"
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

BEGIN_OBJECT( MNPhongBlinn );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNPhongBlinn::MNPhongBlinn( const char* name )
   : MaterialNode( name )
   , m_specularIntensity( NULL )
   , m_specularHardness( NULL )
   , m_normalsVS( NULL )
   , m_outBRDF( NULL )
   , m_outGBufNormals( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      // inputs
      m_specularIntensity = new MSFloatInput( "Specular intensity" );
      m_specularHardness = new MSFloatInput( "Specular hardness" );
      m_normalsVS = new MSVec3Input( "NormalsVS" );

      std::vector< GBNodeInput< MaterialNode >* > inputs;
      inputs.push_back( m_specularIntensity );
      inputs.push_back( m_specularHardness );
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

MNPhongBlinn::MNPhongBlinn( const MNPhongBlinn& rhs )
   : MaterialNode( rhs )
   , m_specularIntensity( NULL )
   , m_specularHardness( NULL )
   , m_normalsVS( NULL )
   , m_outBRDF( NULL )
   , m_outGBufNormals( NULL )
{
   m_specularIntensity = static_cast< MSFloatInput* >( findInput( "Specular intensity" ) );
   m_specularHardness = static_cast< MSFloatInput* >( findInput( "Specular hardness" ) );
   m_normalsVS = static_cast< MSVec3Input* >( findInput( "NormalsVS" ) );

   m_outBRDF = static_cast< MSColorOutput* >( findOutput( "GBuf_BRDF" ) );
   m_outGBufNormals = static_cast< MSColorOutput* >( findOutput( "GBuf_Normals" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNPhongBlinn::~MNPhongBlinn()
{
   m_specularIntensity = NULL;
   m_specularHardness = NULL;
   m_normalsVS = NULL;
   m_outBRDF = NULL;
   m_outGBufNormals = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNPhongBlinn::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing inputs
   m_specularIntensity = static_cast< MSFloatInput* >( findInput( "Specular intensity" ) );
   m_specularHardness = static_cast< MSFloatInput* >( findInput( "Specular hardness" ) );
   m_normalsVS = static_cast< MSVec3Input* >( findInput( "NormalsVS" ) );

   m_outBRDF = static_cast< MSColorOutput* >( findOutput( "GBuf_BRDF" ) );
   m_outGBufNormals = static_cast< MSColorOutput* >( findOutput( "GBuf_Normals" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNPhongBlinn::buildMaterialShader( MaterialShaderBuilder& builder )
{
   // BRDF definition
   if ( m_outBRDF && m_specularIntensity && m_specularHardness )
   {
      const char* gBufBRDFVar = builder.addVariable( m_outBRDF );

      builder.addCodeLine( "<type_Color> %s = <type_Color>(0);", gBufBRDFVar );
      builder.addCodeLine( "%s.x = 0.1;\t\t // BLINN_PHONG_SHADING", gBufBRDFVar );

      if ( m_specularIntensity->isConnected() )
      {
         const char* specularIntensity = builder.getVariable( m_specularIntensity );
         builder.addCodeLine( "%s.y = %s;", gBufBRDFVar, specularIntensity );
      }
      else
      {
         builder.addCodeLine( "%s.y = 0.2;", gBufBRDFVar );
      }

      if ( m_specularHardness->isConnected() )
      {
         const char* specularHardness = builder.getVariable( m_specularHardness );
         builder.addCodeLine( "%s.z = %s / 255.0;", gBufBRDFVar, specularHardness );
      }
      else
      {
         builder.addCodeLine( "%s.z = 20.0 / 255.0;", gBufBRDFVar );
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
