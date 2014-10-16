#include "core-Renderer\MNMixRGBs.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core\ReflectionEnum.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( MNRGBOperation );
   ENUM_VAL( RGBO_Linear_Light );
   ENUM_VAL( RGBO_Soft_Light );
   ENUM_VAL( RGBO_Color );
   ENUM_VAL( RGBO_Saturation );
   ENUM_VAL( RGBO_Hue );
   ENUM_VAL( RGBO_Burn );
   ENUM_VAL( RGBO_Dodge );
   ENUM_VAL( RGBO_Overlay );
   ENUM_VAL( RGBO_Lighten );
   ENUM_VAL( RGBO_Darken );
   ENUM_VAL( RGBO_Difference );
   ENUM_VAL( RGBO_Divide );
   ENUM_VAL( RGBO_Screen );
   ENUM_VAL( RGBO_Subtract );
   ENUM_VAL( RGBO_Multiply );
   ENUM_VAL( RGBO_Add );
   ENUM_VAL( RGBO_Mix );
END_ENUM();

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNMixRGBs );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "Operation", MNRGBOperation, m_operation );
   PROPERTY_EDIT( "Factor", float, m_factor );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNMixRGBs::MNMixRGBs( const char* name, MNRGBOperation operation, float factor )
   : MaterialNode( name )
   , m_operation( operation )
   , m_factor( factor )
   , m_input1( NULL )
   , m_input2( NULL )
   , m_output( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_input1 = new MSColorInput( "In1" );
      m_input2 = new MSColorInput( "In2" );

      std::vector< GBNodeInput< MaterialNode >* > inputs;
      inputs.push_back( m_input1 );
      inputs.push_back( m_input2 );
      defineInputs( inputs );

      m_output = new MSColorOutput( "Color" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNMixRGBs::MNMixRGBs( const MNMixRGBs& rhs )
   : MaterialNode( rhs )
   , m_operation( rhs.m_operation )
   , m_factor( rhs.m_factor )
   , m_input1( NULL )
   , m_input2( NULL )
   , m_output( NULL )
{
   // find the existing sockets
   m_input1 = static_cast< MSColorInput* >( findInput( "In1" ) );
   m_input2 = static_cast< MSColorInput* >( findInput( "In2" ) );
   m_output = static_cast< MSColorOutput* >( findOutput( "Color" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNMixRGBs::~MNMixRGBs()
{
   m_input1 = NULL;
   m_input2 = NULL;
   m_output = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNMixRGBs::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing sockets
   m_input1 = static_cast< MSColorInput* >( findInput( "In1" ) );
   m_input2 = static_cast< MSColorInput* >( findInput( "In2" ) );
   m_output = static_cast< MSColorOutput* >( findOutput( "Color" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNMixRGBs::buildMaterialShader( MaterialShaderBuilder& builder )
{
   MaterialFunction* function = builder.getFunction( "MixRGBs" );
   if ( !function )
   {
      MatFuncUserDefined* functionDef = builder.addFunction( "MixRGBs", 2 );
      function = functionDef;

      std::string param0, param1;
      functionDef->addInputParam( m_input1, param0 );
      functionDef->addInputParam( m_input2, param1 );
      functionDef->setReturnType( m_output );

      switch ( m_operation )
      {
      case RGBO_Linear_Light:
      {
         functionDef->addCodeLine( "<type_Color> outColor = ( %s > 0.5) * ( %s + 2 * ( %s - 0.5 ) ) + ( %s <= 0.5) * ( %s + 2 * %s - 1 );", param0.c_str(), param1.c_str(), param0.c_str(), param0.c_str(), param1.c_str(), param0.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Soft_Light:
      {
         functionDef->addCodeLine( "<type_Color> outColor = ( %s > 0.5 ) * ( 1 - ( 1 - %s ) * ( 1 - ( %s - 0.5 ) ) ) + ( %s <= 0.5 ) * ( %s * ( %s + 0.5 ) );", param0.c_str(), param1.c_str(), param0.c_str(), param0.c_str(), param1.c_str(), param0.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Color:
      {

         break;
      }

      case RGBO_Saturation:
      {
         break;
      }

      case RGBO_Hue:
      {
         break;
      }

      case RGBO_Burn:
      {
         functionDef->addCodeLine( "<type_Color> outColor = 1 - ( 1 - %s ) / %s;", param1.c_str(), param0.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Dodge:
      {
         functionDef->addCodeLine( "<type_Color> outColor = %s / ( 1 - %s );", param1.c_str(), param0.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Overlay:
      {
         functionDef->addCodeLine( "<type_Color> outColor = ( %s > 0.5 ) * ( 1 - ( 1 - 2 * ( %s - 0.5 ) ) * ( 1 - %s ) ) + ( %s <= 0.5 ) * ( ( 2 * %s ) * %s );", param1.c_str(), param1.c_str(), param0.c_str(), param1.c_str(), param1.c_str(), param0.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Lighten:
      {
         functionDef->addCodeLine( "<type_Color> outColor = max( %s, %s );", param0.c_str(), param1.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Darken:
      {
         functionDef->addCodeLine( "<type_Color> outColor = min( %s, %s );", param0.c_str(), param1.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Difference:
      {
         functionDef->addCodeLine( "<type_Color> outColor = abs( %s - %s );", param1.c_str(), param0.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Divide:
      {
         functionDef->addCodeLine( "<type_Color> outColor = %s / %s;", param0.c_str(), param1.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Screen:
      {
         functionDef->addCodeLine( "<type_Color> outColor = 1 - (1 - %s ) * ( 1 - %s ) ;", param1.c_str(), param0.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Subtract:
      {
         functionDef->addCodeLine( "<type_Color> outColor = %s - %s;", param0.c_str(), param1.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Multiply:
      {
         functionDef->addCodeLine( "<type_Color> outColor = %s * %s;", param0.c_str(), param1.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Add:
      {
         functionDef->addCodeLine( "<type_Color> outColor = %s + %s;", param0.c_str(), param1.c_str() );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }

      case RGBO_Mix: // fallthrough
      default:
      {

         functionDef->addCodeLine( "<type_Color> outColor = mix( %s, %s, %f );", param0.c_str(), param1.c_str(), m_factor );
         functionDef->addCodeLine( "return outColor;" );
         break;
      }
      }
   }

   MaterialFunctionCall* functionCall = builder.callFunction( function );
   functionCall->bindInput( m_input1 );
   functionCall->bindInput( m_input2 );
   functionCall->bindOutput( m_output );
}

///////////////////////////////////////////////////////////////////////////////
