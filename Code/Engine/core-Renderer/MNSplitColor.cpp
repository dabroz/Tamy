#include "core-Renderer\MNSplitColor.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core\ReflectionEnum.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNSplitColor );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNSplitColor::MNSplitColor( const char* name )
   : MaterialNode( name )
   , m_input( NULL )
   , m_outRed( NULL )
   , m_outGreen( NULL )
   , m_outBlue( NULL )
   , m_outAlpha( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_input = new MSColorInput( "Color" );
      defineInput( m_input );

      m_outRed = new MSFloatOutput( "r" );
      m_outGreen = new MSFloatOutput( "g" );
      m_outBlue = new MSFloatOutput( "b" );
      m_outAlpha = new MSFloatOutput( "a" );
      
      std::vector< GBNodeOutput< MaterialNode >* > outputs;
      outputs.push_back( m_outRed );
      outputs.push_back( m_outGreen );
      outputs.push_back( m_outBlue );
      outputs.push_back( m_outAlpha );

      defineOutputs( outputs );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNSplitColor::MNSplitColor( const MNSplitColor& rhs )
   : MaterialNode( rhs )
   , m_input( NULL )
   , m_outRed( NULL )
   , m_outGreen( NULL )
   , m_outBlue( NULL )
   , m_outAlpha( NULL )
{
   // find the existing sockets
   m_input = static_cast< MSColorInput* >( findInput( "Color" ) );
   m_outRed = static_cast< MSFloatOutput* >( findOutput( "r" ) );
   m_outGreen = static_cast< MSFloatOutput* >( findOutput( "g" ) );
   m_outBlue = static_cast< MSFloatOutput* >( findOutput( "b" ) );
   m_outAlpha = static_cast< MSFloatOutput* >( findOutput( "a" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNSplitColor::~MNSplitColor()
{
   m_input = NULL;
   m_outRed = NULL;
   m_outGreen = NULL;
   m_outBlue = NULL;
   m_outAlpha = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNSplitColor::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing sockets
   m_input = static_cast< MSColorInput* >( findInput( "Color" ) );
   m_outRed = static_cast< MSFloatOutput* >( findOutput( "r" ) );
   m_outGreen = static_cast< MSFloatOutput* >( findOutput( "g" ) );
   m_outBlue = static_cast< MSFloatOutput* >( findOutput( "b" ) );
   m_outAlpha = static_cast< MSFloatOutput* >( findOutput( "a" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNSplitColor::buildMaterialShader( MaterialShaderBuilder& builder )
{
   if ( m_input->isConnected() )
   {
      const char* varInColor = builder.getVariable( m_input );
      const char* varR = builder.addVariable( m_outRed );
      const char* varG = builder.addVariable( m_outGreen );
      const char* varB = builder.addVariable( m_outBlue );
      const char* varA = builder.addVariable( m_outAlpha );

      builder.addCodeLine( "float %s, %s, %s, %s;", varR, varG, varB, varA );
      builder.addCodeLine( "%s = %s.r; %s = %s.g; %s = %s.b; %s = %s.a;", varR, varInColor, varG, varInColor, varB, varInColor, varA, varInColor );
   }
}

///////////////////////////////////////////////////////////////////////////////
