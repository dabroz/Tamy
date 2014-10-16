/// @file   TamyEditor/GraphWidgetUtils.h
/// @brief  various utility methods used by the pipeline widget
#pragma once

#include <QtGui\QPainter>


///////////////////////////////////////////////////////////////////////////////

/**
 * Various utility methods used by the pipeline widget.
 */
class GraphWidgetUtils
{
public:
   static QPen s_textPen;
   static QPen s_textShadowPen;
   static QPen s_borderPen;
   static QPen s_highlightedBorderPen;
   static QPen s_connectionPen;

   static float g_socketWidth;
   static float g_socketHeight;

public:
   /**
    * A utility method for drawing a text with shadow behind it.
    */
   static void drawShadowedText( QPainter* painter, const QRectF& rect, const QString& text, const QTextOption& options );
};

///////////////////////////////////////////////////////////////////////////////
