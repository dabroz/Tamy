/// @file   core-Renderer/RQWorldPosition.h
/// @brief  this query will return a position in the scene that corresponds to the selected position in the viewport
#pragma once

#include "core-Renderer\RendererQuery.h"
#include "core-Renderer\RenderCommand.h"
#include "core\Point.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class RendererSelectionListener;
class RenderTarget2D;

///////////////////////////////////////////////////////////////////////////////

/**
 * This query will return a position in the scene that corresponds to the selected position in the viewport.
 */
class RQWorldPosition : public RendererQuery
{
   DECLARE_ALLOCATOR( RQWorldPosition, AM_ALIGNED_16 );

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
   RQWorldPosition( const Point& viewportPos, RendererSelectionListener* listener );

   // -------------------------------------------------------------------------
   // RendererQuery implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer, RenderTarget2D* queryRT );
};

///////////////////////////////////////////////////////////////////////////////
/**
 * A special render command used to select entities visible in the viewport.
 */
class RCQueryWorldPosition : public RenderCommand
{
   DECLARE_ALLOCATOR( RCQueryWorldPosition, AM_DEFAULT );

private:
   Point                         m_viewportPos;
   const RenderTarget2D*         m_renderTarget;
   RendererSelectionListener*    m_listener;

public:
   /**
    * Constructor.
    *
    * @param viewportPos
    * @param listener
    */
   RCQueryWorldPosition( const Point& viewportPos, const RenderTarget2D* renderTarget, RendererSelectionListener* listener );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Marshals the results of a world position query.
 */
class RCMarshallWorldPositionQueryResults : public RenderCommand
{
   DECLARE_ALLOCATOR( RCMarshallWorldPositionQueryResults, AM_ALIGNED_16 );

public:
   RendererSelectionListener*       m_listener;
   Vector                           m_worldPos;

public:
   /**
    * Constructor.
    *
    * @param listener
    */
   RCMarshallWorldPositionQueryResults( RendererSelectionListener* listener );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
