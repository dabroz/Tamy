#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderSystem.h"
#include "core-Renderer\RendererImplementation.h"
#include "core-Renderer\RenderingMechanism.h"
#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Viewport.h"
#include "core-Renderer\InternalRenderCommands.h"
#include "core-Renderer\RenderingContext.h"
#include "core-Renderer\RenderStateFlags.h"
#include "core-Renderer\RenderStateChangeTracker.h"

// renderer queries
#include "core-Renderer\RendererQuery.h"

// render tree
#include "core-Renderer\RenderTree.h"
#include "core-Renderer\GeometryComponent.h"

// transforms update system
#include "core-MVC\TransformsManagementSystem.h"

// utils
#include "core\ListUtils.h"
#include "core\Thread.h"
#include "core\Assert.h"
#include "core\MatrixUtils.h"
#include "core\Profiler.h"
#include "core\TSContinuousMemoryPool.h"
#include "core\ContinuousMemoryPool.h"
#include "core\FragmentedMemoryPool.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class NullRenderingMechanism : public RenderingMechanism
   {
   public:
      void initialize( Renderer& renderer ) {}
      void deinitialize( Renderer& renderer ) {}
      void render( Renderer& renderer ) {}
   };

} // namespace anonymous

///////////////////////////////////////////////////////////////////////////////

Renderer::Renderer( RendererImplementation* implementation, uint viewportWidth, uint viewportHeight )
   : m_implementation( implementation )
   , m_viewportWidth( viewportWidth )
   , m_viewportHeight( viewportHeight )
   , m_queriesQueue( new RoundBuffer( 1024 ) )
   , m_mechanism( new NullRenderingMechanism() )
   , m_renderTreeMemPool( new ContinuousMemoryPool( 1024 * 1024 ) )
   , m_renderStateCommandsQueue( new FragmentedMemoryPool( 128 * 1024, 64 ) )
   , m_renderStateFlags( new RenderStateFlags() )
   , m_renderThreadCommandsQueue( NULL )
   , m_commandsDataAllocator( NULL )
   , m_context( new RenderingContext() )
{
   // state changes trackers
   m_stateChangeTrackersStack = new Stack< RenderStateChangeTracker* >( m_renderStateCommandsQueue );

   // cameras
   m_defaultCamera = new Camera( "defaultCamera", Camera::PT_PERSPECTIVE );
   m_camerasStack.push( m_defaultCamera );

   // start updating the camera
   TransformsManagementSystem& tmSys = TSingleton< TransformsManagementSystem >::getInstance();
   tmSys.addTransformable( m_defaultCamera );

   MatrixUtils::generateViewportMatrix( 0, 0, m_viewportWidth, m_viewportHeight, m_viewportMatrix );

   // initialize the implementation
   m_implementation->initialize( *this );

   // register renderer with the render system
   {
      RenderSystem& renderSystem = TSingleton< RenderSystem >::getInstance();
      renderSystem.registerRenderer( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

Renderer::~Renderer()
{
   // unregister renderer from the render system
   {
      RenderSystem& renderSystem = TSingleton< RenderSystem >::getInstance();
      renderSystem.unregisterRenderer( this );
   }

   // delete the render target dedicated to selection queries
   {
      delete m_queriesQueue;
      m_queriesQueue = NULL;
   }

   // delete the implementation
   m_implementation->deinitialize( *this );
   delete m_implementation;
   m_implementation = NULL;

   // delete remaining mechanisms used by the renderer
   if ( m_mechanism )
   {
      m_mechanism->deinitialize( *this );
   }

   // stop updating camera transforms
   if ( !m_camerasStack.empty() )
   {
      TransformsManagementSystem& tmSys = TSingleton< TransformsManagementSystem >::getInstance();
      tmSys.removeTransformable( m_camerasStack.top() );
   }

   delete m_context;
   m_context = NULL;

   delete m_mechanism;
   m_mechanism = NULL;

   delete m_stateChangeTrackersStack;
   m_stateChangeTrackersStack = NULL;

   delete m_renderStateFlags;
   m_renderStateFlags = NULL;

   delete m_renderStateCommandsQueue;
   m_renderStateCommandsQueue = NULL;

   delete m_renderTreeMemPool;
   m_renderTreeMemPool = NULL;

   delete m_defaultCamera;
   m_defaultCamera = NULL;

   m_renderThreadCommandsQueue = NULL;
   m_commandsDataAllocator = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::initialize( TSContinuousMemoryPool* commandsDataAllocator, SteppedLocklessCommandsQueue< Renderer >* renderThreadCommandsQueue )
{
   ASSERT( !m_commandsDataAllocator && !m_renderThreadCommandsQueue );

   m_commandsDataAllocator = commandsDataAllocator;
   m_renderThreadCommandsQueue = renderThreadCommandsQueue;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::setMechanism( RenderingMechanism* mechanism )
{
   // deinitialize and release the old mechanism
   if ( m_mechanism )
   {
      m_mechanism->deinitialize( *this );
   }
   delete m_mechanism;

   // setup and initialize a new one
   if ( mechanism == NULL )
   {
      m_mechanism = new NullRenderingMechanism();
   }
   else
   {
      m_mechanism = mechanism;
   }
   m_mechanism->initialize( *this );
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::pushCamera( Camera& camera ) 
{ 
   TransformsManagementSystem& tmSys = TSingleton< TransformsManagementSystem >::getInstance();

   // stop updating the previously active camera
   if ( !m_camerasStack.empty() )
   {
      tmSys.removeTransformable( m_camerasStack.top() );
   }
   
   // set the new camera as the currently active one
   m_camerasStack.push( &camera ); 

   // start updating the new camera
   tmSys.addTransformable( &camera );
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::popCamera() 
{ 
   // stop updating the camera that's about to become inactive
   TransformsManagementSystem& tmSys = TSingleton< TransformsManagementSystem >::getInstance();
   tmSys.removeTransformable( m_camerasStack.top() );

   m_camerasStack.pop(); 

   if ( m_camerasStack.empty() ) 
   { 
      m_camerasStack.push( m_defaultCamera ); 
   } 
   // start updating the newly active camera
   tmSys.addTransformable( m_camerasStack.top() );
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::render()
{
   PROFILED();

   // start rendering
   new ( m_renderThreadCommandsQueue ) RCRenderingBegin( m_viewportWidth, m_viewportHeight );

   // execute the refresh commands
   if ( !m_refreshCommands.empty() )
   {
      for ( List< RefreshCommand >::iterator it = m_refreshCommands.begin(); !it.isEnd(); ++it )
      {
         RefreshCommand& command = *it;
         new ( m_renderThreadCommandsQueue ) RCRefreshResource( command.m_storage, command.m_resource );
      }
      m_refreshCommands.clear();
   }

   // render the scene
   m_mechanism->render( *this );

   // finish rendering
   new ( m_renderThreadCommandsQueue ) RCRenderingEnd();
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::waitForFence()
{
   while( m_renderThreadCommandsQueue->getCommandsCount() > 1 )
   {
      Thread::yield();
   }
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::resizeViewport( uint width, uint height )
{
   if ( m_viewportWidth == width && m_viewportHeight == height )
   {
      // dimensions haven't changed
      return;
   }

   m_viewportWidth = width;
   m_viewportHeight = height;
   MatrixUtils::generateViewportMatrix( 0, 0, m_viewportWidth, m_viewportHeight, m_viewportMatrix );

   // notify listeners
   for ( Stack< Camera* >::iterator it = m_camerasStack.begin(); !it.isEnd(); ++it )
   {
      Camera* camera = *it;
      camera->onViewportResized( m_viewportWidth, m_viewportHeight );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::getViewport( Viewport& outViewportDesc )
{
   outViewportDesc.m_minZ = 0.0f;
   outViewportDesc.m_maxZ = 1.0f;
   outViewportDesc.m_offsetX = 0;
   outViewportDesc.m_offsetY = 0;
   outViewportDesc.m_width = m_viewportWidth;
   outViewportDesc.m_height = m_viewportHeight;
}

///////////////////////////////////////////////////////////////////////////////

RenderTree* Renderer::buildRenderTree( const Array< GeometryComponent* >& visibleElems )
{
   if ( visibleElems.empty() )
   {
      return NULL;
   }

   return new ( m_renderTreeMemPool ) RenderTree( *this, visibleElems );
}

///////////////////////////////////////////////////////////////////////////////

// @MainThread
RenderStateChangeTracker* Renderer::pushStateTracker()
{
   RenderStateChangeTracker* newTracker = new ( m_renderStateCommandsQueue ) RenderStateChangeTracker( *this );

   m_stateChangeTrackersStack->push( newTracker );
   return newTracker;
}

///////////////////////////////////////////////////////////////////////////////

// @MainThread
void Renderer::popStateTracker()
{
   ASSERT( !m_stateChangeTrackersStack->empty() );
   RenderStateChangeTracker* tracker = m_stateChangeTrackersStack->top();
   m_stateChangeTrackersStack->pop();
   delete tracker;
}

///////////////////////////////////////////////////////////////////////////////

// @MainThread
void Renderer::scheduleRefreshCommand( IRenderResourceStorage* storage, RenderResource* resource )
{
   m_refreshCommands.pushBack( RefreshCommand( storage, resource ) );
}

///////////////////////////////////////////////////////////////////////////////
