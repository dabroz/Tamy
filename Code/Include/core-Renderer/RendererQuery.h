/// @file   core-Renderer/RendererQuery.h
/// @brief  renderer query interface
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class RenderTarget2D;

///////////////////////////////////////////////////////////////////////////////

class RendererQuery
{
public:
   virtual ~RendererQuery() {}

   /**
    * Executes the query.
    *
    * @param renderer
    * @param queryRT
    */
   virtual void execute( Renderer& renderer, RenderTarget2D* queryRT ) = 0;
};

///////////////////////////////////////////////////////////////////////////////
