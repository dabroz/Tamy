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
