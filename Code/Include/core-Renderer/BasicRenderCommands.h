/// @file   core-Renderer/BasicRenderCommands.h
/// @brief  basic rendering commands marshaled to the underlying graphics library
#pragma once

#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\RenderingParams.h"
#include "core-Renderer\TextureSamplerSettings.h"
#include "core\Color.h"
#include "core\types.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

enum GeometryFillMode
{
   GFM_Point               = 1,
   GFM_Wireframe           = 2,
   GFM_Solid               = 3,
   GFM_ForceByte           = 0x7f, // force 8-bit size enum
};

/**
 * Command defines the way in which the rendered geometry should be drawn.
 */
class RCSetFillMode  : public RenderCommand
{
   DECLARE_ALLOCATOR( RCSetFillMode, AM_DEFAULT );

private:
   GeometryFillMode       m_fillMode;

public:
   /**
    * Constructor.
    *
    * @param fillMode
    */
   RCSetFillMode( GeometryFillMode fillMode ) : m_fillMode( fillMode ) {}

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Render command that begins the process of rendering a scene.
 */
class RCBeginScene : public RenderCommand
{
   DECLARE_ALLOCATOR( RCBeginScene, AM_DEFAULT );

public:
   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Render command that ends the process of rendering a scene.
 */
class RCEndScene : public RenderCommand
{
   DECLARE_ALLOCATOR( RCEndScene, AM_DEFAULT );

public:
   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

class RCStateTest : public RenderCommand
{
   DECLARE_ALLOCATOR( RCStateTest, AM_DEFAULT );

public:
   bool        m_zTest;
   bool        m_zWrite;

public:
   /**
    * Constructor.
    */
   RCStateTest()
      : m_zTest( false )
      , m_zWrite( false )
   {}

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );

};

///////////////////////////////////////////////////////////////////////////////
