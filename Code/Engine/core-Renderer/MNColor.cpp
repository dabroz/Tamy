#include "core-Renderer\MNColor.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MNColor );
   PARENT( MaterialNode );
   PROPERTY_EDIT( "Color", Color, m_color );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MNColor::MNColor( const char* name, const Color& color )
   : MaterialNode( name )
   , m_color( color )
   , m_output( NULL )
{
   if ( !IS_BEING_SERIALIZED() )
   {
      m_output = new MSColorOutput( "Color" );
      defineOutput( m_output );
   }
}

///////////////////////////////////////////////////////////////////////////////

MNColor::MNColor( const MNColor& rhs )
   : MaterialNode( rhs )
   , m_color( rhs.m_color )
   , m_output( NULL )
{
   m_output = static_cast< MSColorOutput* >( findOutput( "Color" ) );
}

///////////////////////////////////////////////////////////////////////////////

MNColor::~MNColor()
{
   m_output = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MNColor::onObjectLoaded()
{
   MaterialNode::onObjectLoaded();

   // find the existing inputs
   m_output = static_cast< MSColorOutput* >( findOutput( "Color" ) );
}

///////////////////////////////////////////////////////////////////////////////

void MNColor::buildMaterialShader( MaterialShaderBuilder& builder )
{
   builder.addConstant( m_output, m_color );
}

///////////////////////////////////////////////////////////////////////////////
