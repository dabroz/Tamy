#include "gl-Renderer\GLRenderer.h"
#include "core\Log.h"

// singleton resources
#include "gl-Renderer\GLRFullscreenQuad.h"
#include "gl-Renderer\GLRProceduralGeometryRenderer.h"

// OpenGL related headers
#include "GL\glew.h"
#include "GL\wglew.h"
#include "GL\GL.h"
#include "GL\GLU.h"


/////////////////////////////////////////////////////////////////////////////

#define ADD_RESOURCE_STORAGE( Class, Member )  Member = new Class( renderer );  m_storages.push_back( Member );

///////////////////////////////////////////////////////////////////////////////

GLRenderer::GLRenderer( HWND renderWindowHandle )
   : m_hwnd( renderWindowHandle )
   , m_hrc( NULL )
   , m_currWidth( 0 )
   , m_currHeight( 0 )
{
   m_hdc = GetDC( m_hwnd );
}

///////////////////////////////////////////////////////////////////////////////

GLRenderer::~GLRenderer()
{
   ASSERT_MSG( m_storages.empty(), "Render resources storages are not empty - implementation hasn't been deinitialized." );
   m_storages.clear();
}

///////////////////////////////////////////////////////////////////////////////

void GLRenderer::initialize( Renderer& renderer )
{
   m_currWidth = 0;
   m_currHeight = 0;

   // create resource storages
   ADD_RESOURCE_STORAGE( TriangleMeshesStorage, m_triMeshes );
   ADD_RESOURCE_STORAGE( ShadersStorage, m_shaders );
   ADD_RESOURCE_STORAGE( TexturesStorage, m_textures );
   ADD_RESOURCE_STORAGE( CubeTexturesStorage, m_cubeTextures );
   ADD_RESOURCE_STORAGE( ProceduralTexturesStorage, m_proceduralTextures );
   ADD_RESOURCE_STORAGE( RenderTargets2DStorage, m_renderTargets2D );
   ADD_RESOURCE_STORAGE( RenderTargetsCubeStorage, m_renderTargetsCube );
   ADD_RESOURCE_STORAGE( ShaderDataBufferStorage, m_shaderDataBuffers );
}

///////////////////////////////////////////////////////////////////////////////


void GLRenderer::deinitialize( Renderer& renderer )
{
   // release resource storages
   const uint count = m_storages.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_storages[i]->clear( renderer );
      delete m_storages[i];
   }
   m_storages.clear();

   delete m_fullscreenQuad;
   m_fullscreenQuad = NULL;

   delete m_mutableMesh;
   m_mutableMesh = NULL;

   // Remove the rendering context from our device context  
   wglMakeCurrent( m_hdc, 0 );

   // Delete our rendering context  
   wglDeleteContext( m_hrc );

   // Release the device context from our window  
   ReleaseDC( m_hwnd, m_hdc );
}

///////////////////////////////////////////////////////////////////////////////

void GLRenderer::bindRenderingContext( Renderer& renderer )
{
   if ( m_hrc == NULL )
   {
      return;
   }
   // Bind the render context
   wglMakeCurrent( m_hdc, m_hrc );

   // resize the viewport if needed
   const uint width = renderer.getViewportWidth();
   const uint height = renderer.getViewportHeight();
   resizeViewport( width, height );

   // Set the viewport size to fill the window  
   glViewport( 0, 0, width, height );

}

///////////////////////////////////////////////////////////////////////////////

void GLRenderer::unbindRenderingContext( Renderer& renderer )
{
#ifndef _DEBUG
   // Double buffering is disabled in the debug mode, because it was causing stalls.
   // The explanation and an associated TODO is described in GLRenderer.cpp : 135
   SwapBuffers( m_hdc );
#endif
}

///////////////////////////////////////////////////////////////////////////////

void GLRenderer::createDisplay()
{
   // Create a new PIXELFORMATDESCRIPTOR (PFD)
   PIXELFORMATDESCRIPTOR pfd;
   {
      memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
      pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR ); // Set the size of the PFD to the size of the class  

#ifdef _DEBUG
      // no double buffering support in the debug mode - for some reason it stalls the pipeline. The Debug Heap might
      // be the cause of it, but I  also experienced the stall when I ran the compiled Debug app from HD,
      // so it's not necessarily that.
      // Anyway - everything works fine without doubler buffering in this mode, so for the moment I'm gonna stick to this solution
      //
      // TODO: once everything else works fine, find out what exactly is causing the stalls when double buffering is enabled

      pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window  
#else
      pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window  
#endif
      pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels  
      pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)  
      pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)  
      pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD  
   }

   int nPixelFormat = ChoosePixelFormat( m_hdc, &pfd );
   if ( nPixelFormat == 0 )
   {
      LOG( "Failed to select that pixel format" );
      return;
   }

   BOOL bResult = SetPixelFormat( m_hdc, nPixelFormat, &pfd );
   if ( !bResult )
   {
      LOG( "Failed to set the specified pixel format" );
      return;
   }

   // We need to start by create an OpenGL 2.1 context for our device context, and later on upgrading it to the 3.x standard
   HGLRC tempOpenGLContext = wglCreateContext( m_hdc );

   // Make the OpenGL 2.1 context current and active  
   wglMakeCurrent( m_hdc, tempOpenGLContext );

   // enable the extensions manager
   GLenum error = glewInit();
   if ( error != GLEW_OK )
   {
      LOG( "Failed to initialize the extensions manager" );
      return;
   }

   // now we can finally switch to OpenGL 3.x
   int attributes[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3, // Set the MAJOR version of OpenGL to 3  
      WGL_CONTEXT_MINOR_VERSION_ARB, 3, // Set the MINOR version of OpenGL to 3 
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible  
      0
   };

   // If the OpenGL 3.x context creation extension is available  
   if ( wglewIsSupported( "WGL_ARB_create_context" ) == 1 )
   {
      // Create and OpenGL 3.x context based on the given attributes  
      m_hrc = wglCreateContextAttribsARB( m_hdc, NULL, attributes );

      // Remove the temporary context from being active  
      wglMakeCurrent( NULL, NULL );

      // Delete the temporary OpenGL 2.1 context  
      wglDeleteContext( tempOpenGLContext );

      // Make our OpenGL 3.0 context current  
      wglMakeCurrent( m_hdc, m_hrc );

      LOG( "OpenGL 3.x context initialized" );
   }
   else
   {
      // If we didn't have support for OpenGL 3.x and up, use the OpenGL 2.1 context 
      LOG( "OpenGL 3.x initialization failed, we're sticking to OpenGL 2.1" );
      m_hrc = tempOpenGLContext;
   }

   // let's check which version did we manage to initialize
   {
      int glVersion[2] = { -1, -1 }; // Set some default values for the version  
      glGetIntegerv( GL_MAJOR_VERSION, &glVersion[0] ); // Get back the OpenGL MAJOR version we are using  
      glGetIntegerv( GL_MINOR_VERSION, &glVersion[1] ); // Get back the OpenGL MAJOR version we are using  

      LOG( "Using OpenGL %d.%d", glVersion[0], glVersion[1] );
   }

   // setup the state that won't change
   {
      // face culling
      glEnable( GL_CULL_FACE );
      glFrontFace( GL_CW );
      glCullFace( GL_BACK );

      // initial depth buffer usage
      glEnable( GL_DEPTH_TEST );
      glDepthMask( true );

      // default clear color
      glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
   }

   // setup basic components
   {
      m_fullscreenQuad = new GLRFullscreenQuad();
      m_mutableMesh = new GLRMutableMesh();
   }
}

/////////////////////////////////////////////////////////////////////////////

void GLRenderer::resizeViewport( uint width, uint height )
{
   if ( width == m_currWidth && height == m_currHeight )
   {
      // the dimensions haven't changed
      return;
   }

   // memorize the new dimensions
   m_currWidth = width;
   m_currHeight = height;

   // update the dimensions of dynamic render targets
   Array< GLRRenderTarget2D* >& renderTargets = m_renderTargets2D->getImplementations();
   uint count = renderTargets.size();
   for ( uint i = 0; i < count; ++i )
   {
      GLRRenderTarget2D* rt = renderTargets[i];
      if ( rt )
      {
         rt->onViewportResized( width, height );
      }
   }
}

/////////////////////////////////////////////////////////////////////////////

GLRTriangleMesh* GLRenderer::getTriangleMesh( const TriangleMesh* mesh )
{
   return m_triMeshes->getInstance( mesh );
}

///////////////////////////////////////////////////////////////////////////////

GLRShader* GLRenderer::getShader( const Shader* shader )
{
   return m_shaders->getInstance( shader );
}

/////////////////////////////////////////////////////////////////////////////

uint GLRenderer::getTexture( const Texture* texture )
{
   return m_textures->getInstance( texture );
}

/////////////////////////////////////////////////////////////////////////////

uint GLRenderer::getCubeTexture( const CubeTexture* texture )
{
   return m_cubeTextures->getInstance( texture );
}

/////////////////////////////////////////////////////////////////////////////

uint GLRenderer::getProceduralTexture( const ProceduralTexture* texture )
{
   return m_proceduralTextures->getInstance( texture );
}

/////////////////////////////////////////////////////////////////////////////

GLRRenderTarget2D* GLRenderer::getRenderTarget( const RenderTarget2D* renderTarget )
{
   return m_renderTargets2D->getInstance( renderTarget );
}

/////////////////////////////////////////////////////////////////////////////

GLRRenderTargetCube* GLRenderer::getRenderTarget( const RenderTargetCube* renderTarget )
{
   return m_renderTargetsCube->getInstance( renderTarget );
}

/////////////////////////////////////////////////////////////////////////////

uint GLRenderer::getShaderDataBuffer( const ShaderDataBuffer* dataBuffer )
{
   return m_shaderDataBuffers->getInstance( dataBuffer );
}

/////////////////////////////////////////////////////////////////////////////
