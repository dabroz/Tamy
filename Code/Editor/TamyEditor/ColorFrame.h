/// @file   TamyEditor\ColorFrame.h
/// @brief  Widget showing a color
#pragma once

#include <QtWidgets\QLabel>


///////////////////////////////////////////////////////////////////////////////

class Color;

///////////////////////////////////////////////////////////////////////////////

/**
 * Widget showing a color.
 */
class ColorFrame : public QLabel
{
   Q_OBJECT

private:
   QColor            m_color;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param color      engine color
    */
   ColorFrame( QWidget* parent, const Color& color );

   /**
    * Constructor.
    *
    * @param parent
    * @param color      qt color
    */
   ColorFrame( QWidget* parent, const QColor& color );

   /**
    * Returns the displayed color value.
    */
   QColor getColor() const;

   /**
    * Returns the displayed engine color value.
    */
   Color getEngineColor() const;

   /**
    * Sets the new color value.
    */
   void setColor( const Color& color );

   /**
    * Opens a color picker and allows the user to select a color.
    */
   void selectColor();

signals:
   /**
    * Signal emitted when the color value changes.
    */
   void changed( const QColor& );

protected:
   void paintEvent( QPaintEvent* event );
};

///////////////////////////////////////////////////////////////////////////////
