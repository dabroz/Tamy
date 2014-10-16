/// @file   core-Renderer\RendererUtils.h
/// @brief  various utility methods used by the rendering pipelines
#pragma once


///////////////////////////////////////////////////////////////////////////////

class RendererUtils
{
private:
   union PtrAsBytes
   {
      long ptr;
      struct
      {
         unsigned char b[4];
      };
   };

public:
   /**
    * Calculates the boundaries of the scene.
    *
    * @param scene
    * @param outBounds
    */
   static void calculateSceneBounds( const class Model& scene, struct AxisAlignedBox& outBounds );

   /**
    * Converts a regular pointer to a vector, which can be used as an argument
    * of the SceneQueryEffect.
    *
    * @param ptr           pointer we want to convert
    * @pram outVec        pointer in a vector form
    */
   static void ptrToVec( void* ptr, struct Vector& outVec );

   /**
    * Inverted conversion to the one introduced by method 'ptrToVec'
    *
    * @param vec     a vector representation of a pointer
    * @return        a pointer
    */
   static void* vecToPtr( const struct Vector& vec );
};

///////////////////////////////////////////////////////////////////////////////
