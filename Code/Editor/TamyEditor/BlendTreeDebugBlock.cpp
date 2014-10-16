#include "BlendTreeDebugBlock.h"
#include "core-AI\BlendTreeNode.h"
#include "core-AI\BlendTreeAnimation.h"
#include "core-AI\SnapshotAnimation.h"
#include <QtWidgets\QGraphicsSceneHoverEvent>
#include <QtGui\QPainter>
#include <QtGui\QPainterPath>


///////////////////////////////////////////////////////////////////////////////

BlendTreeDebugBlock::BlendTreeDebugBlock( const BlendTreeNode& node )
   : m_node( node )
   , m_duration( 0.0f )
   , m_nodeName( node.getName().c_str() )
   , m_fillBrush( QPointF(0, 0), QPointF( 0, 36 ) )
{
   setFlags( QGraphicsItem::ItemIsSelectable );
   setAcceptHoverEvents( true );
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeDebugBlock::initialize( float blockHeight, float startTime, float trackOffset )
{
   setPos( QPointF( startTime, trackOffset ) );

   m_globalBounds.setTopLeft( QPointF( 0, 0 ) );
   m_globalBounds.setWidth( 1.0f );
   m_globalBounds.setHeight( blockHeight );

   m_fillBrush.setColorAt( 0, QColor( 255, 182, 104 ) );
   m_fillBrush.setColorAt( 0.1f, QColor( 255, 110, 20 ) );
}

///////////////////////////////////////////////////////////////////////////////

bool BlendTreeDebugBlock::isEqual( const BlendTreeDebugBlock* block, const BlendTreeNode* node )
{
   return block->getNode() == node;
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeDebugBlock::updateWidth( BlendTreePlayer* player, float timeElapsed )
{
   m_duration += timeElapsed;
   m_globalBounds.setWidth( m_duration );

   update();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeDebugBlock::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );

   // block
   {
      QPainterPath blockRectPath;
      blockRectPath.addRoundedRect( m_globalBounds, 2.0f, 2.0f );

      painter->setBrush( m_fillBrush );
      painter->drawPath( blockRectPath );
   }

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeDebugBlock::hoverEnterEvent( QGraphicsSceneHoverEvent* event )
{
   setToolTip( m_nodeName );
   event->accept();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

QPen BTDBAnimationNode::s_trackTimePen(QColor( 45, 133, 255 ), 2, Qt::SolidLine );

///////////////////////////////////////////////////////////////////////////////

BTDBAnimationNode::BTDBAnimationNode( const BlendTreeAnimation& node )
   : BlendTreeDebugBlock( node )
   , m_animationNode( node )
   , m_animationDuration( node.getAnimation() ? node.getAnimation()->m_duration * 100.0f : 0.0f )
   , m_playbackSpeed( 1.0f )
{
   m_trackTimesRecord.append( QPointF( 0, 0 ) );
   m_trackTimesRecord.append( QPointF( 0, 0 ) );
}

///////////////////////////////////////////////////////////////////////////////

void BTDBAnimationNode::updateWidth( BlendTreePlayer* player, float timeElapsed )
{
   BlendTreeDebugBlock::updateWidth( player, timeElapsed );

   if ( m_animationDuration > 0.0f )
   {
      float playbackSpeed = m_animationNode.getPlaybackSpeed( player );
      float trackTime = m_animationNode.getTrackTime( player ) * 100.0f;

      QPointF& prevProgressPoint = m_trackTimesRecord.back();
      float progress = trackTime / m_animationDuration;

      const bool hasSpeedChanged = fabs( m_playbackSpeed - playbackSpeed ) > 1e-3f;
      const bool hasClipLoopedAround = prevProgressPoint.y() > progress;
      if ( fabs( m_playbackSpeed - playbackSpeed ) > 1e-3f ) 
      {
         // playback speed has changed - create a new track record point
         m_trackTimesRecord.append( QPointF( m_duration, progress ) );

         // record the new playback speed
         m_playbackSpeed = playbackSpeed;
      }
      else if ( prevProgressPoint.y() > progress )
      {
         // the clip has looped around. Insert 2 new points.
         // One, to mark the end of the previous clip, and a new one, to start working fresh

         m_trackTimesRecord.back() = QPointF( m_duration, 1.0f );
         m_trackTimesRecord.append( QPointF( m_duration, 0.0f ) );
         m_trackTimesRecord.append( QPointF( m_duration, 0.0f ) );
      }
      else
      {
         // update the value of the previous track record point
         m_trackTimesRecord.back() = QPointF( m_duration, progress );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BTDBAnimationNode::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
   BlendTreeDebugBlock::paint( painter, option, widget );

   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );

   // track time chart
   {
      QPainterPath trackTimeChartPath;

      float height = m_globalBounds.height() - 2;

      trackTimeChartPath.moveTo( m_trackTimesRecord.front() );
      foreach ( QPointF timePoint, m_trackTimesRecord )
      {
         trackTimeChartPath.lineTo( QPointF( timePoint.x(), height * ( 1.0f - timePoint.y() ) + 1 ) );
      }

      painter->setPen( s_trackTimePen );
      painter->drawPath( trackTimeChartPath );
   }

   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////
