/// @file   core-Renderer/RQRenderTargetContents.h
/// @brief  this query returns the contents of the specified render target
#pragma once

#include "core-Renderer\RendererQuery.h"
#include "core-Renderer\RenderCommand.h"


///////////////////////////////////////////////////////////////////////////////

class RenderTarget2D;
class SurfaceQuery;

///////////////////////////////////////////////////////////////////////////////

class RQRenderTargetContents : public RendererQuery
{
   DECLARE_ALLOCATOR( RQRenderTargetContents, AM_DEFAULT );

private:
   SurfaceQuery*                    m_query;

public:
   /**
    * Constructor.
    *
    * @param query
    */
   RQRenderTargetContents( SurfaceQuery* query );
   ~RQRenderTargetContents();

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer, RenderTarget2D* queryRT );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Copies the entire surface of the specified render target to the specified query's buffer.
 *
 * The target NEEDS to be readable.
 */
class RCGetRenderTargetContents : public RenderCommand
{
   DECLARE_ALLOCATOR( RCGetRenderTargetContents, AM_DEFAULT );

private:
   const RenderTarget2D*            m_renderTarget;
   SurfaceQuery*                    m_query;

public:
   /**
    * Constructor.
    *
    * @param renderTarget        queried render target
    * @param query
    */
   RCGetRenderTargetContents( const RenderTarget2D* renderTarget, SurfaceQuery* query );
   ~RCGetRenderTargetContents();

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
