#include "core-Renderer\TextField.h"
#include "core-Renderer\Font.h"
#include "core-Renderer\Renderer.h"


///////////////////////////////////////////////////////////////////////////////

TextField::TextField( Renderer& renderer )
: m_renderer( renderer )
, m_font(NULL)
, m_screenWidth(1)
, m_screenHeight(1)
, m_x(0)
, m_y(0)
, m_width(0)
, m_height(0)
, m_color( 1.f, 1.f, 1.f, 1.f )
, m_justification(TJ_LEFT)
{
   recalculateOverlayRect();
}

///////////////////////////////////////////////////////////////////////////////

TextField::~TextField()
{
}

///////////////////////////////////////////////////////////////////////////////

void TextField::setText( const std::string& text )
{
   m_text = text;
}

///////////////////////////////////////////////////////////////////////////////

void TextField::setSize( float x, float y, float width, float height )
{
   m_x = x;
   m_y = y;
   m_width = width;
   m_height = height;
   recalculateOverlayRect();
}

///////////////////////////////////////////////////////////////////////////////

void TextField::setColor( const Color& color )
{
   m_color = color;
}

///////////////////////////////////////////////////////////////////////////////

void TextField::setJustification( TextJustification justification )
{
   m_justification = justification;
}

///////////////////////////////////////////////////////////////////////////////

void TextField::setFont( Font& font )
{
   m_font = &font;
}

///////////////////////////////////////////////////////////////////////////////

void TextField::render( Renderer& renderer )
{
   if (m_font != NULL)
   {
      new ( renderer.rtComm() ) RCDrawText( m_font, m_text, m_overlaySize, m_justification, m_color );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TextField::update( Renderer& renderer )
{
   m_screenWidth = (float)renderer.getViewportWidth();
   m_screenHeight = (float)renderer.getViewportHeight();
   recalculateOverlayRect();
}

///////////////////////////////////////////////////////////////////////////////

void TextField::update( Renderer& renderer, const RendererOps& operation )
{
}

///////////////////////////////////////////////////////////////////////////////

void TextField::recalculateOverlayRect()
{
   m_overlaySize.left = (long)(m_x * m_screenWidth);
   m_overlaySize.top = (long)(m_y * m_screenHeight);
   m_overlaySize.right = (long)(m_overlaySize.left + (m_width * m_screenWidth));
   m_overlaySize.bottom = (long)(m_overlaySize.top + (m_height * m_screenWidth));
}

///////////////////////////////////////////////////////////////////////////////
