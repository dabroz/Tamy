/// @file   core-Renderer/ShaderUtils.h
/// @brief
#pragma once


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class ShaderTexture;
struct Vector;

///////////////////////////////////////////////////////////////////////////////

/**
 * A set of utility methods used when playing with various shaders.
 */
class ShaderUtils
{
public:
   /**
    * Calculates the size of the half pixel for the specified texture.
    *
    * @param renderer
    * @param inputTex
    * @param halfPixel
    */
   static void calculateHalfPixel( Renderer& renderer, ShaderTexture* inputTex, Vector& halfPixel );

   /**
    * Swaps the code of the specified shader routine for another one.
    *
    * @param routineIdx
    * @param shader
    * @param routinePath      path to one of the scripts. The method expects to find 2 script
    *                         filed next to one another - an HLSL and a GLSL scripts with extensions .hlsl and.glsl respectively
    *                         This path is a path to any of them - the method will adjust the extensions when needed
    */
   static void swapRoutineCode( enum ShaderRoutine routineIdx, class Shader* shader, const class FilePath& routinePath );
};

///////////////////////////////////////////////////////////////////////////////
