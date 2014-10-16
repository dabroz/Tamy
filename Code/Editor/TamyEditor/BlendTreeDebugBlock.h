/// @file   TamyEditor/BlendTreeDebugBlock.h
/// @brief  a block displayed by the TimetrackDebugger that represents a BlendTreeNode' activity record
#pragma once

#include <QtWidgets\QGraphicsItem>
#include <QLinearGradient>
#include <QList>
#include <QPointF>


///////////////////////////////////////////////////////////////////////////////

class BlendTreeNode;
class BlendTreeAnimation;
class BlendTreePlayer;

///////////////////////////////////////////////////////////////////////////////

/**
 * A block displayed by the TimetrackDebugger that represents a BlendTreeNode's activity record.
 */
class BlendTreeDebugBlock : public QGraphicsItem
{
protected:
   const BlendTreeNode&          m_node;
   float                         m_duration;

   QString                       m_nodeName;

   QLinearGradient               m_fillBrush;
   QRectF                        m_globalBounds;

public:
   /**
    * Constructor.
    *
    * @param node
    */
   BlendTreeDebugBlock( const BlendTreeNode& node );

   /**
    * Initializes the block.
    * 
    * @param blockHeight
    * @param startTime
    * @param trackOffset
    */
   void initialize( float blockHeight, float startTime, float trackOffset );

   /**
    * Returns the node this block represents.
    */
   inline const BlendTreeNode* getNode() const { return &m_node; }
   
   /**
    * Updates the node's bounds and duration.
    *
    * @param player
    * @param timeElapsed
    */
   virtual void updateWidth( BlendTreePlayer* player, float timeElapsed );

   /**
    * A comparator used by ListUtils::find method.
    *
    * @param block
    * @param node
    */
   static bool isEqual( const BlendTreeDebugBlock* block, const BlendTreeNode* node );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_globalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );
   void hoverEnterEvent( QGraphicsSceneHoverEvent* event );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A block displayed by the TimetrackDebugger that represents a BlendTreeAnimation node's activity record.
 */
class BTDBAnimationNode : public BlendTreeDebugBlock
{
private:
   static QPen                   s_trackTimePen;

   const BlendTreeAnimation&     m_animationNode;
   const float                   m_animationDuration;

   float                         m_playbackSpeed;
   QList< QPointF >              m_trackTimesRecord;

public:
   /**
    * Constructor.
    *
    * @param node
    */
   BTDBAnimationNode( const BlendTreeAnimation& node );

   // -------------------------------------------------------------------------
   // BlendTreeDebugBlock implementation
   // -------------------------------------------------------------------------
   void updateWidth( BlendTreePlayer* player, float timeElapsed );

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );
};

///////////////////////////////////////////////////////////////////////////////
