/// @file   core-Renderer/RenderCommand.h
/// @brief  a render command interface
#pragma once

#include "core\MemoryRouter.h"
#include "core\LocklessCommand.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;

///////////////////////////////////////////////////////////////////////////////

/**
 * A renderer command issues implementation specific rendering methods.
 */

class RenderCommand : public LocklessCommand< Renderer >
{
public:
   virtual ~RenderCommand() {}

};

///////////////////////////////////////////////////////////////////////////////
