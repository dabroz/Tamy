/// @file   TamyEditor/BlendTreeItemsRenderers.h
/// @brief  a set of methods that renders various elements displayed by a BlendTree editor
#pragma once


///////////////////////////////////////////////////////////////////////////////

class QPainter;
class QRectF;

///////////////////////////////////////////////////////////////////////////////

/**
 * A set of methods that renders various elements displayed by a BlendTree editor.
 */
class BlendTreeItemsRenderers
{
public:
   /**
    * Paints a graphical representation of a BlendTreeEvent.
    *
    * @param painter
    * @param globalBounds
    */
   static void paintEvent( QPainter* painter, const QRectF& globalBounds );

   /**
    * Paints a graphical representation of a BlendTreeSyncPoint.
    *
    * @param painter
    * @param globalBounds
    * @param opaqueness
    */
   static void paintSyncPoint( QPainter* painter, const QRectF& globalBounds, float opaqueness = 1.0f );
};

///////////////////////////////////////////////////////////////////////////////
