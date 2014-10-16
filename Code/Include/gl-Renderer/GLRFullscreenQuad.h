/// @file   gl-Renderer\GLRFullscreenQuad.h
/// @brief  an OpenGL representation of a full screen quad
#pragma once

#include "core\MemoryRouter.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class GLRFullscreenQuad
{
   DECLARE_ALLOCATOR( GLRFullscreenQuad, AM_DEFAULT );

private:
   uint                          m_vaoID;
   uint                          m_vboVerticesID;
   uint                          m_vboIndicesID;

public:
   /**
    * Constructor.
    */
   GLRFullscreenQuad();
   ~GLRFullscreenQuad();

   /**
    * Renders the quad on the currently active device;
    */
   void render() const;
};

///////////////////////////////////////////////////////////////////////////////
