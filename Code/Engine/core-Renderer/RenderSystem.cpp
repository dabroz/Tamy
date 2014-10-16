#include "core-Renderer\RenderSystem.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RendererImplementation.h"
#include "core-Renderer\InternalRenderCommands.h"

// threads
#include "core\ThreadSystem.h"
#include "core\Thread.h"
#include "core\CriticalSection.h"

// memory
#include "core\TSContinuousMemoryPool.h"

// utils
#include "core\ListUtils.h"
#include "core\Profiler.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

// 1 MB for the render thread commands
#define RENDER_THREAD_QUEUE_SIZE          1024*1024
#define MAIN_THREAD_QUEUE_SIZE            20*1024

// and 1MB for any extra allocations render commands make
#define RENDER_MEMORY_SIZE                2024*1024

///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( RenderSystem );

///////////////////////////////////////////////////////////////////////////////

RenderSystem::RenderSystem( const SingletonConstruct& )
   : m_runRenderingThread( true )
   , m_rendererRegistrationsPending( false )
   , m_renderingInProgress( false )
{
   // create the rendering thread
   ThreadSystem& threadSys = TSingleton< ThreadSystem >::getInstance();
   m_mainRenderingThread = &threadSys.getCurrentThread();
   m_renderingThread = new Thread();

   bool wasThreadStarted = m_renderingThread->start( *this );
   ASSERT_MSG( wasThreadStarted, "Unable to start the rendering thread" );

   m_renderersListLock = new CriticalSection();
}

///////////////////////////////////////////////////////////////////////////////

RenderSystem::~RenderSystem()
{
   m_runRenderingThread = false;
   m_renderingThread->join(); 

   delete m_renderingThread;
   m_renderingThread = NULL;

   delete m_renderersListLock;
   m_renderersListLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

uint RenderSystem::getMainThreadId() const
{
   return m_mainRenderingThread->getThreadId();
}

///////////////////////////////////////////////////////////////////////////////

uint RenderSystem::getRenderThreadId() const
{
   return m_renderingThread->getThreadId();
}

///////////////////////////////////////////////////////////////////////////////

bool RenderSystem::isCalledFromMainThread() const 
{
   return m_mainRenderingThread->isCurrentThread();
}

///////////////////////////////////////////////////////////////////////////////

// @main thread
void RenderSystem::registerRenderer( Renderer* renderer )
{
   if ( !renderer )
   {
      return;
   }

   bool isRegistered = false;
   const uint count = m_renderers.size();
   for ( uint i = 0; i < count; ++i )
   {
      Renderer* registeredRenderer = m_renderers[i]->m_renderer;
      if ( registeredRenderer == renderer )
      {
         isRegistered = true;
         break;
      }
   }
   if ( isRegistered )
   {
      ASSERT_MSG( false, "This renderer is already registered." );
      return;
   }

   // add a new entry
   {
      // signal the rendering thread that it needs to pause until we can remove 
      // the renderer
      m_rendererRegistrationsPending = true;

      // wait until the rendering thread finishes the loop
      while ( m_renderingInProgress )
      {
         Thread::yield();
      }

      // create and add the new entry
      RendererEntry* entry = new RendererEntry( renderer, this );
      m_renderers.push_back( entry );

      // resume the rendering thread's loop
      m_rendererRegistrationsPending = false;

   }
}

///////////////////////////////////////////////////////////////////////////////

// @main thread
void RenderSystem::unregisterRenderer( Renderer* renderer )
{
   if ( !renderer )
   {
      return;
   }

   // check if we even have such a renderer on our list
   RendererEntry* removedEntry = NULL;
   {
      // signal the rendering thread that it needs to pause until we can remove 
      // the renderer
      m_rendererRegistrationsPending = true;

      const int count = m_renderers.size();
      for ( int i = count - 1; i >= 0; --i )
      {
         RendererEntry* entry = m_renderers[i];
         if ( entry->m_renderer == renderer )
         {
            // wait until the rendering thread finishes the loop
            while( m_renderingInProgress )
            {
               Thread::yield();
            }

            // remove the entry from the list
            removedEntry = entry;

            removedEntry->m_renderThreadCommandsQueue->setWriterThreadId( m_mainRenderingThread->getThreadId() );
            removedEntry->m_renderThreadCommandsQueue->setReaderThreadId( m_mainRenderingThread->getThreadId() );
            removedEntry->m_mainThreadCommandsQueue->setWriterThreadId( m_mainRenderingThread->getThreadId() );
            removedEntry->m_mainThreadCommandsQueue->setReaderThreadId( m_mainRenderingThread->getThreadId() );

            m_renderers.remove( i );
            
            break;
         }
      }

      // resume the rendering thread's loop
      m_rendererRegistrationsPending = false;
   }

   // process the remaining commands here
   if ( removedEntry )
   {
      Renderer* removedRenderer = removedEntry->m_renderer;

      removedEntry->m_renderThreadCommandsQueue->flush();
      removedEntry->m_renderThreadCommandsQueue->process( *removedRenderer );

      removedEntry->m_mainThreadCommandsQueue->flush();
      removedEntry->m_mainThreadCommandsQueue->process( *removedRenderer );

      // and finally delete the entry
      delete removedEntry;
   }
}

///////////////////////////////////////////////////////////////////////////////

// @main thread
void RenderSystem::flush()
{
   // render
   const uint count = m_renderers.size();
   for ( uint i = 0; i < count; ++i )
   {
      RendererEntry* entry = m_renderers[i];
      entry->m_renderThreadCommandsQueue->commit();
   }
}

///////////////////////////////////////////////////////////////////////////////

// @main thread
void RenderSystem::render()
{
   // render
   const uint count = m_renderers.size();
   for ( uint i = 0; i < count; ++i )
   {
      RendererEntry* entry = m_renderers[i];

      // the old batch of commands hasn't been processed yet
      if ( entry->m_renderThreadCommandsQueue->getCommandsCount() <= 1 )
      {
         // process the commands destined for the main thread
         entry->m_mainThreadCommandsQueue->process( *entry->m_renderer );

         entry->m_renderer->render();
      }
      entry->m_renderThreadCommandsQueue->commit();

      DEBUG_CODE( 
         uint renderCommandsCount = entry->m_renderThreadCommandsQueue->getMemoryUsed();
         PROFILE_VALUE( uint, renderCommandsCount );
      );
   }
}

///////////////////////////////////////////////////////////////////////////////

/// @Rendering thread
void RenderSystem::run()
{
   while ( m_runRenderingThread )
   {
      while ( m_rendererRegistrationsPending )
      {
         // wait until pending registrations are processed
         Thread::yield();
      }

      m_renderingInProgress = true;
      const uint count = m_renderers.size();
      for ( uint i = 0; i < count; ++i )
      {
         RendererEntry* entry = m_renderers[i];
         entry->m_renderThreadCommandsQueue->process( *entry->m_renderer );
      }
      m_renderingInProgress = false;
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RenderSystem::RendererEntry::RendererEntry( Renderer* renderer, RenderSystem* system )
   : m_renderer( renderer )
{
   // create a memory pool dedicated to keeping the rendering commands related data
   m_commandsDataAllocator = new TSContinuousMemoryPool( RENDER_MEMORY_SIZE );

   // create the commands queue
   m_renderThreadCommandsQueue = new SteppedLocklessCommandsQueue< Renderer >( RENDER_THREAD_QUEUE_SIZE );
   m_mainThreadCommandsQueue = new SteppedLocklessCommandsQueue< Renderer >( MAIN_THREAD_QUEUE_SIZE );

   // setup debug data
   m_renderThreadCommandsQueue->setWriterThreadId( system->m_mainRenderingThread->getThreadId() );
   m_renderThreadCommandsQueue->setReaderThreadId( system->m_renderingThread->getThreadId() );
   m_mainThreadCommandsQueue->setWriterThreadId( system->m_renderingThread->getThreadId() );
   m_mainThreadCommandsQueue->setReaderThreadId( system->m_mainRenderingThread->getThreadId() );

   new ( m_renderThreadCommandsQueue ) RCInitializeRenderer();
   m_renderThreadCommandsQueue->commit();

   // initialize the renderer
   m_renderer->implementation()->setCommandsQueue( m_mainThreadCommandsQueue );
   m_renderer->initialize( m_commandsDataAllocator, m_renderThreadCommandsQueue );
}

///////////////////////////////////////////////////////////////////////////////

RenderSystem::RendererEntry::~RendererEntry()
{
   m_renderer = NULL;

   delete m_commandsDataAllocator;
   m_commandsDataAllocator = NULL;

   delete m_renderThreadCommandsQueue;
   m_renderThreadCommandsQueue = NULL;

   delete m_mainThreadCommandsQueue;
   m_mainThreadCommandsQueue = NULL;
}

///////////////////////////////////////////////////////////////////////////////
