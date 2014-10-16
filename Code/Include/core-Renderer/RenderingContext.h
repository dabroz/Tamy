/// @file   core-Renderer\RenderingContext.h
/// @brief  a blackboard that provides context sensitive data during rendering
#pragma once

#include "core\MemoryRouter.h"
#include "core\Vector.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A blackboard that provides context sensitive data during rendering.
 *
 * Different rendering systems, that may reside in different libraries, and thus not know
 * of one another, can relay precious information between themselves using this blackboard.
 */
struct RenderingContext
{
   DECLARE_ALLOCATOR( RenderingContext, AM_DEFAULT );

   bool                       m_bool;
   int                        m_int;
   float                      m_float;
   Vector                     m_vec4;
   Color                      m_color;
   Matrix                     m_mtx;
   class ShaderTexture*       m_texture;

   class ShaderDataBuffer*    m_dataBuffer;
};

///////////////////////////////////////////////////////////////////////////////
