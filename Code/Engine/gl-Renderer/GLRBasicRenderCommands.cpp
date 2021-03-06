#include "core-Renderer\BasicRenderCommands.h"
#include "gl-Renderer\GLRDefinitions.h"


///////////////////////////////////////////////////////////////////////////////

void RCSetFillMode::execute( Renderer& renderer )
{
   glPolygonMode( GL_FRONT_AND_BACK, glFillMode[m_fillMode] );
};

///////////////////////////////////////////////////////////////////////////////

void RCBeginScene::execute( Renderer& renderer )
{
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

///////////////////////////////////////////////////////////////////////////////

void RCEndScene::execute( Renderer& renderer )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void RCStateTest::execute( Renderer& renderer )
{
}

///////////////////////////////////////////////////////////////////////////////
