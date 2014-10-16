#include "ColorFrame.h"
#include "core\Color.h"
#include <QtWidgets\QColorDialog>
#include <QtGui\QColor>
#include <QtGui\QPainter>
#include <QtGui\QPaintEvent>


///////////////////////////////////////////////////////////////////////////////

ColorFrame::ColorFrame( QWidget* parent, const Color& color )
   : QLabel( parent )
   , m_color( color.r * 255, color.g * 255, color.b * 255, color.a * 255 )
{
   setMinimumSize( 20, 20 );
}

///////////////////////////////////////////////////////////////////////////////

ColorFrame::ColorFrame( QWidget* parent, const QColor& color )
   : QLabel( parent )
   , m_color( color )
{
   setMinimumSize( 20, 20 );
}

///////////////////////////////////////////////////////////////////////////////

void ColorFrame::selectColor()
{
   QPalette p = palette();
   
   QColor newColor = QColorDialog::getColor( m_color, NULL, "Select color" );
   if ( newColor.isValid() )
   {
      m_color = newColor;
   }

   update();

   emit changed( m_color );
}

///////////////////////////////////////////////////////////////////////////////

QColor ColorFrame::getColor() const
{
   return m_color;
}

///////////////////////////////////////////////////////////////////////////////

Color ColorFrame::getEngineColor() const
{
   return Color( m_color.redF(), m_color.greenF(), m_color.blueF(), m_color.alphaF() );
}

///////////////////////////////////////////////////////////////////////////////

void ColorFrame::setColor( const Color& color )
{
   m_color = QColor( color.r * 255, color.g * 255, color.b * 255, color.a * 255 );

   update();
   emit changed( m_color );
}

///////////////////////////////////////////////////////////////////////////////

void ColorFrame::paintEvent( QPaintEvent* event )
{
   QPainter painter( this );
   painter.setBrush( QBrush( m_color ) );

   QRectF rect = geometry();
   rect.translate( 2, 2 );
   rect.setWidth( rect.width() - 4 );
   rect.setHeight( rect.height() - 4 );
   painter.drawRoundedRect( rect, 4.0f, 4.0f );
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////
