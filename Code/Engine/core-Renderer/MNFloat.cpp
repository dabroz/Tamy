#include "core-Renderer\MNFloat.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNFloat );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "Value", float, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNFloat::MNFloat()
   : MaterialNode( "floatConst" )
   , m_value( 0.0f )
   , m_output( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_output = new MSFloatOutput( "Value" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNFloat::MNFloat( const MNFloat& rhs )
   : MaterialNode( rhs )
   , m_value( rhs.m_value )
   , m_output( NULL )
{
   m_output = DynamicCast< MSFloatOutput >( findOutput( "Value" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNFloat::~MNFloat()
{
   m_output = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNFloat::onObjectLoaded()
{
   __super::onObjectLoaded();

   // find the existing inputs
   m_output = DynamicCast< MSFloatOutput >( findOutput( "Value" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNFloat::buildMaterialShader( MaterialShaderBuilder& builder )
{
   builder.addConstant( m_output, m_value );
}

///////////////////////////////////////////////////////////////////////////////
