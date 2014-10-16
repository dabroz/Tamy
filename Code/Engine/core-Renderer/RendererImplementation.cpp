#include "core-Renderer\RendererImplementation.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderSystem.h"


///////////////////////////////////////////////////////////////////////////////

RendererImplementation::RendererImplementation()
   : m_mainThreadCommandsQueue( NULL)
{
}

///////////////////////////////////////////////////////////////////////////////

RendererImplementation::~RendererImplementation()
{
   m_mainThreadCommandsQueue = NULL;
}

///////////////////////////////////////////////////////////////////////////////

/// @Main thread
void RendererImplementation::setCommandsQueue( SteppedLocklessCommandsQueue< Renderer >* mainThreadCommandsQueue )
{
   m_mainThreadCommandsQueue = mainThreadCommandsQueue;
}

///////////////////////////////////////////////////////////////////////////////

/// @Rendering thread
void RendererImplementation::commitMainThreadCommands()
{
   m_mainThreadCommandsQueue->commit();
}

///////////////////////////////////////////////////////////////////////////////
