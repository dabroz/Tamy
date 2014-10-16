/// @file   TamyEditor/BlendTreeDebugLabel.h
/// @brief  an item that displays the name of a debugger track that's dedicated to a single BlendTreeNode.
#pragma once

#include <QtWidgets\QGraphicsItem>
#include <QLinearGradient>


///////////////////////////////////////////////////////////////////////////////

class BlendTreeDebugLabel : public QGraphicsItem
{
private:
   static QPen                            s_textPen;
   static QFont                           s_font;

   QString                                m_text;

   QLinearGradient                        m_fillBrush;
   QRectF                                 m_globalBounds;

public:
   /**
    * Constructor.
    *
    * @param text
    * @param blockHeight
    * @param trackOffset
    */
   BlendTreeDebugLabel( const char* text, float blockHeight, float trackOffset );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_globalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );
};

///////////////////////////////////////////////////////////////////////////////
