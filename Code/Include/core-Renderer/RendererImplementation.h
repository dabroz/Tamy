/// @file   core-Renderer\RendererImplementation.h
/// @brief  a renderer implementation
#pragma once

#include "core\types.h"
#include "core\SteppedLocklessCommandsQueue.h"
#include "core-Renderer\RenderResource.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class RenderResource;

///////////////////////////////////////////////////////////////////////////////

/**
 * A platform dependent renderer implementation.
 */
class RendererImplementation
{
private:
   SteppedLocklessCommandsQueue< Renderer >*    m_mainThreadCommandsQueue;

public:
   virtual ~RendererImplementation();

   /**
    * Initializes the implementation.
    *
    * @param renderer      host renderer
    */
   virtual void initialize( Renderer& renderer ) {}

   /**
    * Deinitializes the implementation.
    *
    * @param renderer      host renderer
    */
   virtual void deinitialize( Renderer& renderer ) {}

   /**
    * Activates the rendering context. Called before any of the rendering commands get executed.
    *
    * @param renderer      host renderer
    */
   virtual void bindRenderingContext( Renderer& renderer ) {}

   /**
    * Deactivates the rendering context. Called after all rendering commands for this rendering frame get executed.
    *
    * @param renderer      host renderer
    */
   virtual void unbindRenderingContext( Renderer& renderer ) {}

   // ----------------------------------------------------------------------------
   // Render commands queue
   // ----------------------------------------------------------------------------

   /**
    * Sets a commands queue instance this implementation should use.
    *
    * @param mainThreadCommandsQueue
    */
   void setCommandsQueue( SteppedLocklessCommandsQueue< Renderer >* mainThreadCommandsQueue );

   /**
    * Gives access to the main thread's commands buffer.
    */
   inline MemoryAllocator* mtComm() { return m_mainThreadCommandsQueue; }

   /**
    * Commits a batch of commands destined to be executed on the main thread.
    */
   void commitMainThreadCommands();

protected:
   /**
    * Constructor.
    */
   RendererImplementation();
};

///////////////////////////////////////////////////////////////////////////////
