/// @file   core-Renderer/RenderSystem.h
/// @brief  this class manages the render thread and updates all existing Renderer instances
#pragma once

#include "core\MemoryRouter.h"
#include "core\Runnable.h"
#include "core\Singleton.h"
#include "core\Array.h"
#include "core\SteppedLocklessCommandsQueue.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class Thread;
class CriticalSection;
class TSContinuousMemoryPool;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class manages the render thread and updates all existing Renderer instances.
 */
class RenderSystem : public Runnable
{
   PRIORITY_SINGLETON( 13 );
   DECLARE_ALLOCATOR( RenderSystem, AM_DEFAULT );

   struct RendererEntry
   {
      DECLARE_ALLOCATOR( RendererEntry, AM_DEFAULT );

      Renderer*                                    m_renderer;

      TSContinuousMemoryPool*                      m_commandsDataAllocator;
      SteppedLocklessCommandsQueue< Renderer >*    m_renderThreadCommandsQueue;
      SteppedLocklessCommandsQueue< Renderer >*    m_mainThreadCommandsQueue;

      /**
       * Constructor.
       *
       * @param renderer
       * @param system
       */
      RendererEntry( Renderer* renderer, RenderSystem* system );
      ~RendererEntry();
   };

private:
   bool                       m_rendererRegistrationsPending;
   bool                       m_renderingInProgress;

   Array< RendererEntry* >    m_renderers;
   CriticalSection*           m_renderersListLock;

   bool                       m_runRenderingThread;
   Thread*                    m_mainRenderingThread;
   Thread*                    m_renderingThread;

public:
   /**
    * Constructor.
    */
   RenderSystem( const SingletonConstruct& );
   ~RenderSystem();

   /**
    * Flushes all render command queues.
    */
   void flush();

   /**
    * Makes all existing Renderer instances render the next frame.
    */
   void render();

   /**
    * Registers a renderer.
    *
    * @param renderer
    */
   void registerRenderer( Renderer* renderer );

   /**
    * Unregisters a renderer.
    *
    * @param renderer
    */
   void unregisterRenderer( Renderer* renderer );

   // ----------------------------------------------------------------------------
   // Debug
   // ----------------------------------------------------------------------------
   /**
    * Returns the id of main thread.
    */
   uint getMainThreadId() const;

   /**
    * Returns the id of render thread.
    */
   uint getRenderThreadId() const;

   /**
    * Returns true if the method was called from the main render thread.
    */
   bool isCalledFromMainThread() const;

   // ----------------------------------------------------------------------------
   // Runnable implementation
   // ----------------------------------------------------------------------------
   void run();

private:
   void processRendererRegistrations();
   void renderSingleFrame( RendererEntry* entry );
};

///////////////////////////////////////////////////////////////////////////////
