/// @file   ext-RenderingPipeline\SceneRenderer.h
/// @brief  a scene renderer
#pragma once

#include "core\MemoryRouter.h"
#include "core\Color.h"
#include "core\TVector.h"
#include "core\Vector.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class RenderTarget2D;
class ShaderDataBuffer;
class GeometryComponent;
class Light;

///////////////////////////////////////////////////////////////////////////////

class SceneRenderer
{
   DECLARE_ALLOCATOR( SceneRenderer, AM_DEFAULT );

private:
   Array< GeometryComponent* >*     m_geometry;

public:
   /**
    * Constructor.
    *
    * @param geometry
    */
   SceneRenderer( Array< GeometryComponent* >* geometry );
   ~SceneRenderer();

   /**
   * Renders the color of the scene objects.
   *
   * @param renderer
   * @param outTarget
   */
   void renderColor( Renderer& renderer, RenderTarget2D* outTarget );

   /**
    * Renders scene using a single color.
    *
    * @param renderer
    * @param colorOverride
    * @param outTarget
    */
   void renderColorOverride( Renderer& renderer, const struct Color& colorOverride, RenderTarget2D* outTarget );

   /**
    * Renders pointers to objects visible in the scene.
    *
    * @param renderer
    * @param outTarget
    */
   void renderObjectPointers( Renderer& renderer, RenderTarget2D* outTarget );

private:
   /**
    * Renders the scene contents using the specified profile.
    *
    * @param renderer
    * @param profile
    * @param outTarget
    */
   void render( Renderer& renderer, enum RenderProfileId profile, RenderTarget2D* outTarget );
};

///////////////////////////////////////////////////////////////////////////////
