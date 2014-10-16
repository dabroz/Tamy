/// @file   core-Renderer/RQEntitySelection.h
/// @brief  entity selection renderer queries
#pragma once

#include "core-Renderer\RendererQuery.h"
#include "core-Renderer\RenderCommand.h"
#include "core\List.h"
#include "core\Point.h"


///////////////////////////////////////////////////////////////////////////////

class RenderTarget;
class RendererSelectionListener;
class Entity;

///////////////////////////////////////////////////////////////////////////////

/**
 * This query will select a single entity that was rendered at the specified viewport
 * coordinates
 */
class RQSelectEntity : public RendererQuery
{
   DECLARE_ALLOCATOR( RQSelectEntity, AM_ALIGNED_16 );

private:
   Point                         m_viewportPos;
   RendererSelectionListener*    m_listener;

public:
   /**
    * Constructor.
    *
    * @param viewportPos
    * @param listener
    */
   RQSelectEntity( const Point& viewportPos, RendererSelectionListener* listener );

   // -------------------------------------------------------------------------
   // RendererQuery implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer, RenderTarget2D* queryRT );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This query will select a group of entities that were rendered in the specified part of a viewport.
 */
class RQSelectEntities : public RendererQuery
{
   DECLARE_ALLOCATOR( RQSelectEntities, AM_ALIGNED_16 );

private:
   Point                         m_topLeftCorner;
   Point                         m_bottomRightCorner;
   RendererSelectionListener*    m_listener;

public:
   /**
    * Constructor.
    *
    * @param topLeftCorner
    * @param bottomRightCorner
    * @param listener
    */
   RQSelectEntities( const Point& topLeftCorner, const Point& bottomRightCorner, RendererSelectionListener* listener );

   // -------------------------------------------------------------------------
   // RendererQuery implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer, RenderTarget2D* queryRT );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A special render command used to select entities visible in the viewport.
 */
class RCSelectEntities : public RenderCommand
{
   DECLARE_ALLOCATOR( RCSelectEntities, AM_DEFAULT );

private:
   Point                         m_topLeftCorner;
   Point                         m_bottomRightCorner;
   const RenderTarget2D*         m_renderTarget;
   RendererSelectionListener*    m_listener;

public:
   /**
    * Constructor.
    *
    * @param topLeftCorner
    * @param bottomRightCorner
    * @param selectionQueryRenderTarget
    * @param listener
    */
   RCSelectEntities( const Point& topLeftCorner, const Point& bottomRightCorner, const RenderTarget2D* selectionQueryRenderTarget, RendererSelectionListener* listener );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Marshals the results of an entity selection query.
 */
class RCMarshallSelectionQueryResults : public RenderCommand
{
   DECLARE_ALLOCATOR( RCMarshallSelectionQueryResults, AM_ALIGNED_16 );

public:
   RendererSelectionListener*       m_listener;

   List< Entity* >                  m_selectedEntities;

public:
   /**
    * Constructor.
    *
    * @param listener
    */
   RCMarshallSelectionQueryResults( RendererSelectionListener* listener );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
