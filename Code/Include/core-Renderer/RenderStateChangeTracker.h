/// @file   core-Renderer\RenderStateChangeTracker.h
/// @brief  tracks render state changes
#pragma once

#include "core\MemoryRouter.h"
#include "core\FragmentedMemoryPool.h"
#include "core-Renderer\RenderingParams.h"


///////////////////////////////////////////////////////////////////////////////

class RenderStateCommand;
class Renderer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Render state descriptor - use this class to issue render state changing commands, not the individual
 * commands listed below, which are considered PRIVATE and INTERNAL.
 *
 * The class works as brackets - every method call will issue an immediate state change. 
 * Those states will be memorized, and then, when the instance is deleted, they will
 * issue state-reversion commands, which will roll the render state back to its state from
 * before this class was instantiated.
 */
class RenderStateChangeTracker
{
   DECLARE_ALLOCATOR( RenderStateChangeTracker, AM_DEFAULT );

private:
   Renderer&                     m_renderer;

   RenderStateCommand*           m_chainTail;

public:
   /**
    * Constructor.
    *
    * @param renderer
    */
   RenderStateChangeTracker( Renderer& renderer );
   ~RenderStateChangeTracker();

   /**
    * Commits a command
    */
   void commit( RenderStateCommand* command );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This utility macro creates a render state changing tracker.
 */
#define SAVE_RENDER_STATE( renderer ) RenderStateChangeTracker* __renderStateChangeTracker = renderer.pushStateTracker();

///////////////////////////////////////////////////////////////////////////////

/**
 * This utility macro destroys the most recently created state tracker, reverting all the state changes that were made using it.
 */
#define RESTORE_RENDER_STATE( renderer ) renderer.popStateTracker();

///////////////////////////////////////////////////////////////////////////////

/**
 * A utility macro that facilitates the process of issuing a render state command.
 *
 * The macro assumes there are two variables in its scope:
 *   - stateTracker - a reference to a RenderStateChangeTracker class instance - created by the INIT_RENDER_STATE() macro.
 *   - renderer     - a reference to a Renderer class instance
 */
#define CHANGE_RENDER_STATE( renderer, command ) __renderStateChangeTracker->commit( new ( renderer.rsComm() ) command );

///////////////////////////////////////////////////////////////////////////////
