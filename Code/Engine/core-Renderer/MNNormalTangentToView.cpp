#include "core-Renderer\MNNormalTangentToView.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\Shader.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNNormalTangentToView );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "Strenght", float, m_strenght );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNNormalTangentToView::MNNormalTangentToView( const char* name, float strenght )
   : MaterialNode( name )
   , m_strenght( strenght )
   , m_normalTangentSpace( NULL )
   , m_vertexTangent( NULL )
   , m_vertexNormal( NULL )
   , m_outNormal( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_normalTangentSpace = new MSVec3Input( "NormalTS" );
      m_vertexTangent = new MSVec3Input( "SurfTangent" );
      m_vertexNormal = new MSVec3Input( "SurfNormal" );

      std::vector< GBNodeInput< MaterialNode >* > inputs;
      inputs.push_back( m_normalTangentSpace );
      inputs.push_back( m_vertexTangent );
      inputs.push_back( m_vertexNormal );
      defineInputs( inputs );

      m_outNormal = new MSVec3Output( "Normal" );
      defineOutput( m_outNormal );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNNormalTangentToView::MNNormalTangentToView( const MNNormalTangentToView& rhs )
   : MaterialNode( rhs )
   , m_strenght( rhs.m_strenght )
   , m_normalTangentSpace( NULL )
   , m_vertexTangent( NULL )
   , m_vertexNormal( NULL )
   , m_outNormal( NULL )
{
   m_normalTangentSpace = static_cast< MSVec3Input* >( findInput( "NormalTS" ) );
   m_vertexTangent = static_cast< MSVec3Input* >( findInput( "SurfTangent" ) );
   m_vertexNormal = static_cast< MSVec3Input* >( findInput( "SurfNormal" ) );

   m_outNormal = static_cast< MSVec3Output* >( findOutput( "Normal" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNNormalTangentToView::~MNNormalTangentToView()
{
}

///////////////////////////////////////////////////////////////////////////////

void MNNormalTangentToView::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   m_normalTangentSpace = static_cast< MSVec3Input* >( findInput( "NormalTS" ) );
   m_vertexTangent = static_cast< MSVec3Input* >( findInput( "SurfTangent" ) );
   m_vertexNormal = static_cast< MSVec3Input* >( findInput( "SurfNormal" ) );

   m_outNormal = static_cast< MSVec3Output* >( findOutput( "Normal" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNNormalTangentToView::buildMaterialShader( MaterialShaderBuilder& builder )
{
   std::string strenghtConstName;
   builder.addGlobal< MSFloatInput >( var_strength, strenghtConstName );

   MaterialFunction* function = builder.getFunction( "MNNormalTangentToView" );
   if ( !function )
   {
      MatFuncUserDefined* functionDef = builder.addFunction( "MNNormalTangentToView", 3 );
      function = functionDef;

      std::string normalTangentSpace, vtxTangent, vtxNormal;
      functionDef->addInputParam( m_normalTangentSpace, normalTangentSpace );
      functionDef->addInputParam( m_vertexTangent, vtxTangent );
      functionDef->addInputParam( m_vertexNormal, vtxNormal );
      functionDef->setReturnType( m_outNormal );

      functionDef->addCodeLine( "<type_Vector3> normalVS = normalize( %s );", vtxNormal.c_str() );
      functionDef->addCodeLine( "<type_Vector3> tangentVS = normalize( %s );", vtxTangent.c_str() );
      functionDef->addCodeLine( "<type_Vector3> binormalVS = cross( normalVS, tangentVS );" );
      functionDef->addCodeLine( "<type_Mtx3x3> mtx = <type_Mtx3x3>( tangentVS, binormalVS, normalVS );" );

      functionDef->addCodeLine( "<type_Vector3> surfaceNormalTanSpace = %s.xyz * 2.0 - 1.0;", normalTangentSpace.c_str() );
      functionDef->addCodeLine( "surfaceNormalTanSpace.y *= -1.0;" );

      functionDef->addCodeLine("<type_Vector3> surfaceNormalViewSpace = mtx * surfaceNormalTanSpace;" );

      functionDef->addCodeLine( "<type_Vector3> outNormal = normalize( surfaceNormalViewSpace ) * %s;", strenghtConstName.c_str() );
      functionDef->addCodeLine( "return outNormal;" );
   }

   MaterialFunctionCall* functionCall = builder.callFunction( function );
   functionCall->bindInput( m_normalTangentSpace );
   functionCall->bindInput( m_vertexTangent );
   functionCall->bindInput( m_vertexNormal );
   functionCall->bindOutput( m_outNormal );
}

///////////////////////////////////////////////////////////////////////////////

void MNNormalTangentToView::render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const
{
   constantsBuf->accessData< float >( var_strength ) = m_strenght;
}

///////////////////////////////////////////////////////////////////////////////
