/// @file   core-Renderer\ShaderProfile.h
/// @brief  A helper class that stores shader settings for a specific type of library( DirectX, OpenGL )
#pragma once

#include "core\ReflectionObject.h"
#include "core\Array.h"
#include "core\FilePath.h"
#include "core-Renderer\ShaderConstantDesc.h"
#include "core-Renderer\Defines.h"


///////////////////////////////////////////////////////////////////////////////

/**
* A helper class that stores shader settings for a specific type
* of library ( DirectX, OpenGL )
*/
class ShaderProfile : public ReflectionObject
{
   DECLARE_ALLOCATOR( ShaderProfile, AM_DEFAULT );
   DECLARE_CLASS()

private:
   Array< ShaderConstantDesc >               m_constants;

public:
   /**
    * Default constructor.
    */
   ShaderProfile();

   /**
    * Returns an array of constants the shader operates on.
    */
   inline const Array< ShaderConstantDesc >& getConstants() const {
      return m_constants;
   }

   /**
    * Returns an array of constants the shader operates on ( modifiable version )
    */
   inline Array< ShaderConstantDesc >& accessConstants() {
      return m_constants;
   }

};

///////////////////////////////////////////////////////////////////////////////
