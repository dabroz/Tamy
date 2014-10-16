#include "core-Renderer\MNBool.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNBool );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "Value", bool, m_value )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

MNBool::MNBool()
   : MaterialNode( "boolConst" )
   , m_value( true )
   , m_output( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_output = new MSBoolOutput( "Value" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNBool::MNBool( const MNBool& rhs )
   : MaterialNode( rhs )
   , m_value( rhs.m_value )
   , m_output( NULL )
{
   m_output = static_cast< MSBoolOutput* >( findOutput( "Value" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNBool::~MNBool()
{
   m_output = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNBool::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing inputs
   m_output = static_cast< MSBoolOutput* >( findOutput( "Value" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNBool::buildMaterialShader( MaterialShaderBuilder& builder )
{
   builder.addConstant( m_output, m_value );
}

///////////////////////////////////////////////////////////////////////////////
