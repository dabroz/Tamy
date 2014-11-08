/// @file   core-Renderer/Renderer.h
/// @brief  graphics renderer
#ifndef _RENDERER_H
#define _RENDERER_H

#include "core\List.h"
#include "core\Array.h"
#include "core\Color.h"
#include "core\TimeDependent.h"
#include "core\types.h"
#include "core\MemoryRouter.h"
#include "core\Matrix.h"
#include "core\Stack.h"
#include "core\Point.h"
#include "core\Runnable.h"
#include "core\RoundBuffer.h"
#include "core\SteppedLocklessCommandsQueue.h"
#include <vector>
#include <map>


///////////////////////////////////////////////////////////////////////////////

class RendererImplementation;
class RenderingMechanism;
class Camera;
class RenderCommand;
struct Viewport;
class RenderTree;
class GeometryComponent;
class RendererQuery;
struct RenderingContext;
struct RenderStateFlags;
class RenderStateChangeTracker;
class ContinuousMemoryPool;
class TSContinuousMemoryPool;
class FragmentedMemoryPool;
class IRenderResourceStorage;
class RenderResource;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class implements the engine's rendering strategy.
 * It needs to be implemented in order to actually work with
 * the graphical equipment (the implementation can be done using
 * DirectX, openGL etc. - we don't specify that here)
 *
 * The renderer can be observer - it will notify it's observers about the
 * changes in its state (rendering, device lost, device recovered)
 */
class Renderer
{
   DECLARE_ALLOCATOR( Renderer, AM_DEFAULT );

private:
   struct RefreshCommand
   {
      IRenderResourceStorage*       m_storage;
      RenderResource*               m_resource;

      RefreshCommand( IRenderResourceStorage* storage, RenderResource* resource )
         : m_storage( storage )
         , m_resource( resource )
      {
      }
   };

private:
   // this memory pool is dedicated exclusively to the RenderTree, thus the friendship declaration
   friend class RenderTree;
   ContinuousMemoryPool*                        m_renderTreeMemPool;

private:
   RendererImplementation*                      m_implementation;

   // selection management
   RoundBuffer*                                 m_queriesQueue;

   // rendering mechanism
   RenderingMechanism*                          m_mechanism;

   // camera
   Camera*                                      m_defaultCamera;
   Stack< Camera* >                             m_camerasStack;

   // viewport
   unsigned int                                 m_viewportWidth;
   unsigned int                                 m_viewportHeight;
   Matrix                                       m_viewportMatrix;

   // render commands
   TSContinuousMemoryPool*                      m_commandsDataAllocator;
   SteppedLocklessCommandsQueue< Renderer >*    m_renderThreadCommandsQueue;

   // render state
   FragmentedMemoryPool*                        m_renderStateCommandsQueue;      // this memory pool is dedicated exclusively to render state changing commands, thus the friendship with their instantiator
   RenderStateFlags*                            m_renderStateFlags;
   Stack< RenderStateChangeTracker* >*          m_stateChangeTrackersStack;

   // shared rendering context
   RenderingContext*                            m_context;

   List< RefreshCommand >                       m_refreshCommands;

public:
   /**
    * Constructor.
    *
    * @param implementation
    * @param viewportWidth
    * @param viewportHeight
    */
   Renderer( RendererImplementation* implementation, uint viewportWidth = 800, uint viewportHeight = 600 );
   virtual ~Renderer();

   /**
    * Locks the caller thread until only a single fence command remains in the rendering queue.
    *
    * For testing purposes only.
    */
   void waitForFence();

   /**
    * This method set a new rendering mechanism.
    *
    * @param mech    new mechanism that will be used for rendering.
    */
   void setMechanism( RenderingMechanism* mech );

   /**
    * Returns a renderer implementation.
    */
   inline RendererImplementation* implementation() {
      return m_implementation;
   }

   // -------------------------------------------------------------------------
   // Render tree creation
   // -------------------------------------------------------------------------

   /**
    * Build the render tree.
    *
    * @param visibleElems
    */
   RenderTree* buildRenderTree( const Array< GeometryComponent* >& visibleElems );

   // -------------------------------------------------------------------------
   // Rendering context & state access
   // -------------------------------------------------------------------------
   /**
    * Gives access to the rendering context ( constant version )
    */
   inline const RenderingContext& getContext() const {
      return *m_context;
   }

   /**
   * Gives access to the rendering context ( non-constant version )
   */
   inline RenderingContext& accessContext() {
      return *m_context;
   }

   /**
    * Gives access to the queries queue.
    */
   inline RoundBuffer* accessQueriesQueue() {
      return m_queriesQueue;
   }

   // -------------------------------------------------------------------------
   // Viewport management
   // -------------------------------------------------------------------------

   /**
    * Changes the size of the active viewport.
    *
    * @param width
    * @param height
    */
   void resizeViewport( uint width, uint height );

   /**
    * This method returns the width of currently set viewport
    */
   uint getViewportWidth() const { return m_viewportWidth; }

   /**
    * This method returns the height of currently set viewport
    */
   uint getViewportHeight() const { return m_viewportHeight; }

   /**
    * Returns current viewport matrix.
    */
   inline const Matrix& getViewportMtx() const { return m_viewportMatrix; }

   /**
    * Returns the viewport description.
    *
    * @param outViewportDesc
    */
   void getViewport( Viewport& outViewportDesc );

   // ----------------------------------------------------------------------------
   // Camera management
   // ----------------------------------------------------------------------------
   /**
    * Returns the currently active camera ( const version )
    */
   inline const Camera& getActiveCamera() const { return *m_camerasStack.top(); }

   /**
    * Returns the currently active camera.
    */
   inline Camera& getActiveCamera() { return *m_camerasStack.top(); }

   /**
    * Pushes a camera to the top of the camera stack, thus making it an active camera for this renderer.
    *
    * @param camera
    */
   void pushCamera( Camera& camera );

   /**
    * Pops a camera from the top of the stack, deactivating it.
    * The default camera will never be popped - there always needs to be at least one active camera remaining.
    */
   void popCamera();

   // -------------------------------------------------------------------------
   // Memory pools
   // -------------------------------------------------------------------------
   /**
    * Gives access to the render thread's commands buffer.
    */
   inline MemoryAllocator* rtComm() { return m_renderThreadCommandsQueue; }

   /**
    * Gives access to a designated allocator that manages render thread's rendering commands data related memory
    */
   inline TSContinuousMemoryPool* rtMemPool() {
      return m_commandsDataAllocator;
   }

   /**
    * Gives access to a memory pool dedicated to render state changing commands.
    */
   inline FragmentedMemoryPool* rsComm() {
      return m_renderStateCommandsQueue;
   }

   /**
    * Gives access to the render queries queue.
    */
   inline RoundBuffer* query() { return m_queriesQueue; }

   /**
    * Resource refreshes take place outside the rendering frame.
    * That's why we need to schedule their execution to take place after the rendering frame starts.
    */
   void scheduleRefreshCommand( IRenderResourceStorage* storage, RenderResource* resource );

   // -------------------------------------------------------------------------
   // Render state changes
   // -------------------------------------------------------------------------
   
   /**
    * Gives access to the current setting of the render state.
    */
   inline RenderStateFlags& accessRenderStateFlags()
   {
      return *m_renderStateFlags;
   }

   /**
    * Pushes a new state changes tracker on top of the stack.
    */
   RenderStateChangeTracker* pushStateTracker();

   /**
    * Pops a state changes tracker from top of the stack.
    */
   void popStateTracker();

private:
   friend class RenderSystem;

   /**
    * Initializes the renderer.
    *
    * @param commandsDataAllocator
    * @param renderThreadCommandsQueue
    */
   void initialize( TSContinuousMemoryPool* commandsDataAllocator, SteppedLocklessCommandsQueue< Renderer >* renderThreadCommandsQueue );

   /**
    * Renders the graphical representation of the scene on
    * a graphical device. 
    * The graphical device used is specified in the implementation
    * of this abstract class
    */
   void render();
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\Renderer.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _RENDERER_H
