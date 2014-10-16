#include "core-Renderer\ShaderUtils.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\ShaderTexture.h"
#include "core-Renderer\Shader.h"
#include "core\Vector.h"

// files handling
#include "core\FIlesystem.h"
#include "core\FilePath.h"
#include "core\StreamBuffer.h"


///////////////////////////////////////////////////////////////////////////////

void ShaderUtils::calculateHalfPixel( Renderer& renderer, ShaderTexture* inputTex, Vector& outHalfPixel )
{
   if ( inputTex )
   {
      outHalfPixel[0] = 1.0f / (float)inputTex->getWidth();
      outHalfPixel[1] = 1.0f / (float)inputTex->getHeight();
   }
   else
   {
      outHalfPixel[0] = 1.0f / (float)renderer.getViewportWidth();
      outHalfPixel[1] = 1.0f / (float)renderer.getViewportHeight();
   }
   outHalfPixel[2] = 0.0f;
   outHalfPixel.mul( Float_Inv2 );
}

///////////////////////////////////////////////////////////////////////////////

void ShaderUtils::swapRoutineCode( enum ShaderRoutine routineIdx, Shader* shader, const FilePath& routinePath )
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   File* glslFile = fs.open( routinePath );

   StreamBuffer<char> glslFileReader( *glslFile );

   std::string glslScript = glslFileReader.getBuffer();
   shader->setScript( routineIdx, glslScript );

   delete glslFile;
}

///////////////////////////////////////////////////////////////////////////////
