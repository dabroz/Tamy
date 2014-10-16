#pragma once

#include "core\Color.h"
#include "core\Observer.h"
#include <windows.h>
#include <string>


///////////////////////////////////////////////////////////////////////////////

class Font;
enum TextJustification;
class Renderer;
enum RendererOps;

///////////////////////////////////////////////////////////////////////////////

/**
 * A text field allows to display some text on the screen.
 */
class TextField : public Observer<Renderer, RendererOps>
{
   DECLARE_ALLOCATOR( TextField, AM_DEFAULT );

private:
   Renderer&         m_renderer;
   Font*             m_font;
   std::string       m_text;

   // overlay window dimensions
   float             m_screenWidth;
   float             m_screenHeight;
   float             m_x;
   float             m_y;
   float             m_width;
   float             m_height;
   RECT              m_overlaySize;

   Color             m_color;
   TextJustification m_justification;

public:
   TextField( Renderer& renderer );
   ~TextField();

   /**
    * This method sets the text that should be displayed in the field.
    *
    * @param text    text to be displayed
    */
   void setText( const std::string& text );

   /**
    * This method sets the size of the overlay.
    * Text will be displayed in the bounds of this window.
    * All the params should be specified in 0-1 range.
    *
    * @param x       x position coord of the field
    * @param y       y position coord of the field
    * @param width   field width
    * @param height  field height
    */
   void setSize( float x, float y, float width, float height );

   /**
    * This method sets the color in which the text will be rendered.
    * 
    * @param color   color for the text
    */
   void setColor( const Color& color );

   /**
    * This method sets the justification for the text in this overlay.
    * 
    * @param justification   text justification
    */
   void setJustification( TextJustification justification );

   /**
    * The method sets the font in which the text will be displayed.
    *
    * @param font    font for the text
    */
   void setFont( Font& font );

   // -------------------------------------------------------------------------
   // Overlay implementation
   // -------------------------------------------------------------------------
   void render( Renderer& renderer );

   // -------------------------------------------------------------------------
   // Observer<Renderer, RendererOps> implementation
   // -------------------------------------------------------------------------
   void update( Renderer& renderer );
   void update( Renderer& renderer, const RendererOps& operation );

private:
   void recalculateOverlayRect();
};

///////////////////////////////////////////////////////////////////////////////
