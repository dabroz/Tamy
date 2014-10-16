#include "core-Renderer\MNVec4.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNVec4 );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "vector", Vector, m_vector );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNVec4::MNVec4()
   : MaterialNode( "vecConst" )
   , m_output( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_output = new MSVec4Output( "Vec" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNVec4::MNVec4( const MNVec4& rhs )
   : MaterialNode( rhs )
   , m_vector( rhs.m_vector )
   , m_output( NULL )
{
   m_output = static_cast< MSVec4Output* >( findOutput( "Vec" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNVec4::~MNVec4()
{
   m_output = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNVec4::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing inputs
   m_output = static_cast< MSVec4Output* >( findOutput( "Vec" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNVec4::buildMaterialShader( MaterialShaderBuilder& builder )
{
   builder.addConstant( m_output, m_vector );
}

///////////////////////////////////////////////////////////////////////////////
