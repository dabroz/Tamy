#include "GraphBlock.h"
#include "GraphLayout.h"
#include <QtGui\QPainter>
#include <QtGui\QPainterPath>
#include <QtWidgets\QGraphicsSceneMouseEvent>
#include <QtWidgets\QLabel> 
#include "core\Algorithms.h"
#include <algorithm>
#include "GraphWidgetUtils.h"
#include "GraphBlockSocket.h"


///////////////////////////////////////////////////////////////////////////////

GraphBlock::GraphBlock( GraphLayout* scene )
   : QGraphicsItem( NULL )
   , m_caption( "" )
   , m_scene( scene )
   , m_bounds( QPointF( 0, 0 ), QSizeF( 100, 100 ) )
   , m_captionBounds( QPointF( 0, 0 ), QSizeF( 100, 0 ) )
   , m_font( "Verdana", 15, QFont::Light )
   , m_contentsVisible( false )
   , m_renameWidget( NULL)
   , m_newNameEditor( NULL )
   , m_centralWidget( NULL )
   , m_embeddedWidget( NULL )
   , m_socketsFactory( NULL )
{
   m_font.setStyle( QFont::StyleNormal );
   m_font.setStyleHint( QFont::AnyStyle );
   m_font.setStyleStrategy( QFont::PreferAntialias );

   m_newNameEditor = new BlockCaptionEditor( this );
   m_renameWidget = new QGraphicsProxyWidget( this );
   m_renameWidget->setWidget( m_newNameEditor );

   setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable );

   calculateBounds();
}

///////////////////////////////////////////////////////////////////////////////

GraphBlock::~GraphBlock()
{
   m_socketsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::deinitialize()
{
   QGraphicsScene* parentScene = scene();

   if ( m_centralWidget )
   {
      parentScene->removeItem( m_centralWidget );
      delete m_centralWidget;
      m_centralWidget = NULL;
   }

   if ( m_renameWidget )
   {
      parentScene->removeItem( m_renameWidget );
      delete m_renameWidget;
      m_renameWidget = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::setCentralWidget( QWidget* widget )
{
   m_embeddedWidget = widget;
   if ( !m_centralWidget )
   {
      m_centralWidget = new QGraphicsProxyWidget( this );
   }

   m_centralWidget->setWidget( m_embeddedWidget );

   if ( !m_contentsVisible )
   {
      m_centralWidget->hide();
   }
   calculateBounds();
}

///////////////////////////////////////////////////////////////////////////////

bool GraphBlock::doesOverlap( const QPointF& pos ) const
{
   QRectF bounds( pos, QSizeF( 1, 1 ) );
   return m_bounds.intersects( bounds );
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* event )
{
   QGraphicsItem::mouseDoubleClickEvent( event );

   if ( m_renameWidget )
   {
      m_renameWidget->show();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::setCaption( const QString& caption )
{
   if ( m_renameWidget )
   {
      m_renameWidget->hide();
   }

   m_caption = caption;
   calculateBounds();

   // update the name of the node
   updateNodeName();
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::addSocket( GraphBlockSocket* socket )
{
   if ( !socket )
   {
      return;
   }

   m_sockets.push_back( socket );
   calculateBounds();
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::removeSockets( GraphBlockSocketPosition position, const std::set< std::string >& socketNames )
{
   for ( std::set< std::string >::const_iterator removedSocketNameIt = socketNames.begin(); removedSocketNameIt != socketNames.end(); ++removedSocketNameIt )
   {
      removeSingleSocket( position, *removedSocketNameIt );
   }

   calculateBounds();
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::removeSocket( GraphBlockSocketPosition position, const std::string& socketName )
{
   removeSingleSocket( position, socketName );
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::removeSingleSocket( GraphBlockSocketPosition position, const std::string& socketName )
{
   unsigned int count = m_sockets.size();
   for ( unsigned int socketIdx = 0; socketIdx < count; ++socketIdx )
   {
      GraphBlockSocket* socket = m_sockets[socketIdx];
      if ( socket->getPosition() == position && socket->getName() == socketName )
      {
         // delete the socket
         delete socket;
         m_sockets.erase( m_sockets.begin() + socketIdx );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
   QLinearGradient fillBrush( QPointF( 0, 0 ), QPointF( 0, m_captionBounds.height() ) );
   fillBrush.setColorAt( 0, isSelected() ? QColor( 255, 226, 96 ) : QColor( 200, 200, 200 ) );
   fillBrush.setColorAt( 0.5f, QColor( 70, 70, 70 ) );

   painter->save();

   painter->setRenderHint( QPainter::Antialiasing, true );
   painter->setRenderHint( QPainter::TextAntialiasing, true );
   painter->setFont( m_font );

   const float edgeRadius = 3.0f;

   // draw the block's layout
   if ( m_contentsVisible )
   {
      painter->setPen( GraphWidgetUtils::s_borderPen );

      painter->setBrush( QColor( 150, 150, 150 ) );
      painter->setOpacity( 0.5f );
      painter->drawRoundedRect( m_bounds, edgeRadius, edgeRadius );
   }

   // draw the header
   {

      QPainterPath path;

      QRectF topBB = m_captionBounds;
      topBB.setHeight( m_captionBounds.height() + edgeRadius );

      QRectF bottomBB = m_captionBounds;
      bottomBB.setTop( m_captionBounds.height() );
      bottomBB.setHeight( edgeRadius );

      path.addRoundedRect( topBB, edgeRadius, edgeRadius );
      path.addRect( bottomBB );

      painter->setOpacity( 1.0f );
      painter->setPen( Qt::NoPen );
      painter->setBrush( fillBrush );
      painter->drawPath( path );

      GraphWidgetUtils::drawShadowedText( painter, m_captionBounds, m_caption, QTextOption( Qt::AlignCenter ) );
   }


   painter->restore();
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::calculateBounds()
{
   prepareGeometryChange();

   if ( m_contentsVisible )
   {
      calculateBoundsWithContents();
   }
   else
   {
      calculateEmptyBlockBounds();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::calculateBoundsWithContents()
{
   QFontMetrics metrics( m_font );
   float fontHeight = metrics.height();

   // calculate the size of the caption
   float captionWidth = max2( metrics.width( m_caption ) * 1.5f, 100.0f );
   float captionHeight = fontHeight + 6;

   // go through the sockets, include their width and calculate the required block height
   float longestLeftSocketName = 0;
   float longestRightSocketName = 0;
   unsigned int leftSocketsCount = 0;
   unsigned int rightSocketsCount = 0;
   for ( std::vector< GraphBlockSocket* >::const_iterator it = m_sockets.begin(); it != m_sockets.end(); ++it )
   {
      GraphBlockSocket* socket = *it;
      switch( socket->getPosition() )
      {
      case GBSP_INPUT:
         {
            longestLeftSocketName = max2( longestLeftSocketName, socket->getNameWidth() );
            ++leftSocketsCount;
            break;
         }
      
      case GBSP_OUTPUT:
         {
            longestRightSocketName = max2( longestRightSocketName, socket->getNameWidth() );
            ++rightSocketsCount;
            break;
         }
      }
   }
   float socketSize = metrics.height() + 6;
   float socketsHeight = max2( leftSocketsCount, rightSocketsCount ) * socketSize;

   // calculate the bounds
   QRectF centralWidgetBounds;
   if ( m_centralWidget && m_embeddedWidget )
   {
      centralWidgetBounds = m_centralWidget->subWidgetRect( m_embeddedWidget );
   }

   float blockWidth = max2( captionWidth, longestLeftSocketName + longestRightSocketName );
   blockWidth = max2( blockWidth, (float)centralWidgetBounds.width() ); // make sure the block isn't too narrow
   blockWidth = max2( blockWidth, 100.0f ); // make sure the block isn't too narrow
   float blockHeight = captionHeight + centralWidgetBounds.height() + socketsHeight;

   // set the caption bounds
   m_captionBounds.setWidth( blockWidth );
   m_captionBounds.setHeight( captionHeight );
   m_totalBounds = m_captionBounds;


   // set the block bounds
   m_bounds.setTop( captionHeight );
   m_bounds.setWidth( blockWidth );
   m_bounds.setHeight( blockHeight );

   // calculate the bounds of this block
   m_totalBounds = m_totalBounds.united( m_bounds );

   // set the position of the central widget
   if ( m_renameWidget )
   {
      m_renameWidget->setPos( m_captionBounds.x(), m_captionBounds.y() );
   }
   if ( m_centralWidget )
   {
      m_centralWidget->setPos( ( blockWidth - centralWidgetBounds.width() ) * 0.5f, captionHeight );
   }

   // position the sockets
   float leftSocketsSpacing = ( leftSocketsCount > 0 ) ? ( socketsHeight / (float)leftSocketsCount ) : 0;
   float rightSocketsSpacing = ( rightSocketsCount > 0 ) ? ( socketsHeight / (float)rightSocketsCount ) : 0;

   float rightSocketY = rightSocketsSpacing * 0.5f;
   float leftSocketY = leftSocketsSpacing * 0.5f;
   float socketsVertOffset = captionHeight + centralWidgetBounds.height();
   for ( std::vector< GraphBlockSocket* >::const_iterator it = m_sockets.begin(); it != m_sockets.end(); ++it )
   {
      GraphBlockSocket* socket = *it;

      switch( socket->getPosition() )
      {
      case GBSP_INPUT:
         {
            socket->setCaptionVisible( true );
            socket->setPos( 0, socketsVertOffset + leftSocketY );
            leftSocketY += leftSocketsSpacing;
            break;
         }

      case GBSP_OUTPUT:
         {
            socket->setCaptionVisible( true );
            socket->setPos( blockWidth - GraphWidgetUtils::g_socketWidth, socketsVertOffset + rightSocketY );
            rightSocketY += rightSocketsSpacing;
            break;
         }
      }
   }

   // include the size of the sockets in the block's bounding box
   m_totalBounds = QRectF( m_totalBounds.left() - 11, m_totalBounds.top(), m_totalBounds.right() + 21, m_totalBounds.bottom() );
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::calculateEmptyBlockBounds()
{
   QFontMetrics metrics( m_font );
   float fontHeight = metrics.height();

   // calculate the size of the caption
   float captionWidth = max2( metrics.width( m_caption ) * 1.5f, 100.0f );
   float captionHeight = fontHeight + 6;

   // set the caption bounds
   m_captionBounds.setWidth( captionWidth );
   m_captionBounds.setHeight( captionHeight );
   m_totalBounds = m_captionBounds;

   // we're showing only captions and every socket at the same position
   float socketYPos = captionHeight * 0.5f;
   for ( std::vector< GraphBlockSocket* >::const_iterator it = m_sockets.begin(); it != m_sockets.end(); ++it )
   {
      GraphBlockSocket* socket = *it;

      switch ( socket->getPosition() )
      {
      case GBSP_INPUT:
      {
         socket->setCaptionVisible( false );
         socket->setPos( 0, socketYPos );
         break;
      }

      case GBSP_OUTPUT:
      {
         socket->setCaptionVisible( false );
         socket->setPos( captionWidth - GraphWidgetUtils::g_socketWidth, socketYPos );
         break;
      }
      }
   }

   // include the size of the sockets in the block's bounding box
   m_totalBounds = QRectF( m_totalBounds.left() - 11, m_totalBounds.top(), m_totalBounds.right() + 21, m_totalBounds.bottom() );
}

///////////////////////////////////////////////////////////////////////////////

GraphBlockSocket* GraphBlock::getSocket( GraphBlockSocketPosition position, const std::string& name ) const
{
   unsigned int count = m_sockets.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      GraphBlockSocket* socket = m_sockets[i];
      if ( socket->getPosition() == position && socket->getName() == name )
      {
         // found it
         return socket;
      }
   }

   // socket with such name doesn't exist
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::getAllSockets( GraphBlockSocketPosition position, std::set< std::string >& outSocketNames ) const
{
   unsigned int count = m_sockets.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      GraphBlockSocket* socket = m_sockets[i];
      if ( socket->getPosition() == position  )
      {
         outSocketNames.insert( socket->getName() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GraphBlock::setExpanded( bool expanded )
{
   if ( m_contentsVisible == expanded )
   {
      // nothing changes
      return;
   }

   m_contentsVisible = expanded;
  
   if ( m_centralWidget )
   {
      if ( expanded )
      {
         m_centralWidget->show();
      }
      else
      {
         m_centralWidget->hide();
      }
   }
   
   calculateBounds();
   m_scene->onBlockMoved();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BlockCaptionEditor::BlockCaptionEditor( GraphBlock* block )
   : m_block( block )
{
   connect( this, SIGNAL( editingFinished() ), this, SLOT( blockRenamed() ) );
}

///////////////////////////////////////////////////////////////////////////////

BlockCaptionEditor::~BlockCaptionEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void BlockCaptionEditor::blockRenamed()
{
   m_block->setCaption( text() );
}

///////////////////////////////////////////////////////////////////////////////
