/// @file   core-Renderer/Font.h
/// @brief  a font resource
#pragma once

#include <windows.h>
#include <string>
#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\RenderResource.h"
#include "core\Color.h"


///////////////////////////////////////////////////////////////////////////////

enum TextJustification
{
   TJ_LEFT,
   TJ_CENTERED,
   TJ_RIGHT
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This interface represents a font and its capabilities.
 */
class Font
{
   DECLARE_ALLOCATOR( Font, AM_DEFAULT );

public:
   RenderResource*         m_renderResource;

public:
   /**
    * Constructor.
    */
   Font();
   ~Font();

   // <renderer.todo> implement me
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Text drawing command.
 */
class RCDrawText : public RenderCommand
{
   DECLARE_ALLOCATOR( RCDrawText, AM_DEFAULT );

private:
   const Font*          m_font;
   std::string          m_text;
   RECT                 m_overlaySize;
   TextJustification    m_justification;
   Color                m_color;

public:
   RCDrawText( const Font* font, const std::string& text, const RECT& overlaySize, TextJustification justification, const Color& color );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////