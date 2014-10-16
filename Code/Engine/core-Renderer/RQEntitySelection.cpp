#include "core-Renderer\RQEntitySelection.h"
#include "core-Renderer\RendererSelectionListener.h"
#include "core-Renderer\Renderer.h"


///////////////////////////////////////////////////////////////////////////////

RQSelectEntity::RQSelectEntity( const Point& viewportPos, RendererSelectionListener* listener )
   : m_viewportPos( viewportPos )
   , m_listener( listener )
{
}

///////////////////////////////////////////////////////////////////////////////

void RQSelectEntity::execute( Renderer& renderer, RenderTarget2D* queryRT )
{
   new ( renderer.rtComm() ) RCSelectEntities( m_viewportPos, m_viewportPos, queryRT, m_listener );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RQSelectEntities::RQSelectEntities( const Point& topLeftCorner, const Point& bottomRightCorner, RendererSelectionListener* listener )
   : m_listener( listener )
{
   // make sure the coordinates are correct
   if ( topLeftCorner.x <= bottomRightCorner.x )
   {
      m_topLeftCorner.x = topLeftCorner.x;
      m_bottomRightCorner.x = bottomRightCorner.x;
   }
   else
   {
      m_topLeftCorner.x = bottomRightCorner.x;
      m_bottomRightCorner.x = topLeftCorner.x;
   }

   if ( topLeftCorner.y <= bottomRightCorner.y )
   {
      m_topLeftCorner.y = topLeftCorner.y;
      m_bottomRightCorner.y = bottomRightCorner.y;
   }
   else
   {
      m_topLeftCorner.y = bottomRightCorner.y;
      m_bottomRightCorner.y = topLeftCorner.y;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RQSelectEntities::execute( Renderer& renderer, RenderTarget2D* queryRT )
{
   new ( renderer.rtComm() ) RCSelectEntities( m_topLeftCorner, m_bottomRightCorner, queryRT, m_listener );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RCSelectEntities::RCSelectEntities( const Point& topLeftCorner, const Point& bottomRightCorner, const RenderTarget2D* selectionQueryRenderTarget, RendererSelectionListener* listener )
   : m_topLeftCorner( topLeftCorner )
   , m_bottomRightCorner( bottomRightCorner )
   , m_renderTarget( selectionQueryRenderTarget )
   , m_listener( listener )
{}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RCMarshallSelectionQueryResults::RCMarshallSelectionQueryResults( RendererSelectionListener* listener )
   : m_listener( listener )
{}

///////////////////////////////////////////////////////////////////////////////

void RCMarshallSelectionQueryResults::execute( Renderer& renderer )
{
   m_listener->onEntitiesSelected( m_selectedEntities );
}

///////////////////////////////////////////////////////////////////////////////
