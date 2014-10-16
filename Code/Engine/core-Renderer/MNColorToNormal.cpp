#include "core-Renderer\MNColorToNormal.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\MaterialCodeNode.h"
#include "core-Renderer\Shader.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNColorToNormal );
   PARENT( MaterialNode );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNColorToNormal::MNColorToNormal( const char* name )
   : MaterialNode( name )
   , m_inColor( NULL )
   , m_outNormal( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_inColor = new MSColorInput( "Color" );
      defineInput( m_inColor );

      m_outNormal = new MSVec3Output( "Normal" );
      defineOutput( m_outNormal );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNColorToNormal::MNColorToNormal( const MNColorToNormal& rhs )
   : MaterialNode( rhs )
   , m_inColor( NULL )
   , m_outNormal( NULL )
{
   m_inColor = static_cast< MSColorInput* >( findInput( "Color" ) );
   m_outNormal = static_cast< MSVec3Output* >( findOutput( "Normal" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNColorToNormal::~MNColorToNormal()
{
}

///////////////////////////////////////////////////////////////////////////////

void MNColorToNormal::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   m_inColor = static_cast< MSColorInput* >( findInput( "Color" ) );
   m_outNormal = static_cast< MSVec3Output* >( findOutput( "Normal" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNColorToNormal::buildMaterialShader( MaterialShaderBuilder& builder )
{
   if ( m_inColor->isConnected() )
   {
      const char* inColor = builder.getVariable( m_inColor );
      const char* outNormal = builder.addVariable( m_outNormal );
      builder.addCodeLine( "<type_Vector3> %s = <type_Vector3>( %s.xyz );", outNormal, inColor );
   }
}

///////////////////////////////////////////////////////////////////////////////
