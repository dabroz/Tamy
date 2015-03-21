#include "ext-RenderingPipeline\AmbientOcclusionPass.h"
#include "ext-RenderingPipeline\DeferredRendererGBufferDesc.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\ShaderDataBuffer.h"
#include "core-Renderer\RenderTarget2D.h"
#include "core-Renderer\ShaderUtils.h"
#include "core-Renderer\FullscreenQuad.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\ProceduralTexture.h"
#include "core\Math.h"

// effects
#include "ext-RenderingPipeline\TexturesBlender.h"


///////////////////////////////////////////////////////////////////////////////

AmbientOcclusionPass::AmbientOcclusionPass( RenderTarget2D* intermediateAOTarget )
   : m_shader( FilePath( "/Renderer/Shaders/RenderingPipeline/ambientOcclusion.tsh" ) )
   , m_randomTexture( NULL )
   , m_intermediateAOTarget( intermediateAOTarget )
{
   m_constantsBuf = ShaderDataBuffer::create<AmbientOcclusionSettings>();

   // setup the texture sampler
   m_normalsBufferSampler.setUnified( TAM_BORDER, TFM_POINT );
   m_randomTextureSampler.setUnified( TAM_WRAP, TFM_LINEAR );

   // initialize the settings with default data
   calculateKernel( MAX_AO_KERNEL_SIZE );
   AmbientOcclusionSettings& settings = m_constantsBuf->accessData< AmbientOcclusionSettings >();
   settings.m_power = 1.0f;
   settings.m_radius = 0.3f;

   // initialize the random sampling texture
   m_randomTexture = new ProceduralTexture( 4, 4 );
   {
      PTVector* randomTextureBuf = ( PTVector* ) m_randomTexture->accessImageBuffer();

      Vector tmpPixel;
      for ( int i = 0; i < 16; ++i ) 
      {
         tmpPixel.set( randRange( -1.0f, 1.0 ), randRange( -1.0f, 1.0 ), 0.0f );
         tmpPixel.normalize();

         PTVector& pixel = randomTextureBuf[i];
         pixel.fromVectorAsNormal( tmpPixel );
      }
   }

   // create the blur effect
   m_blurEffect = new TexturesBlender();
}

///////////////////////////////////////////////////////////////////////////////

AmbientOcclusionPass::~AmbientOcclusionPass()
{
   m_randomTexture->removeReference();
   m_randomTexture = NULL;

   m_constantsBuf->removeReference();
   m_constantsBuf = NULL;

   delete m_blurEffect;
   m_blurEffect = NULL;
}

///////////////////////////////////////////////////////////////////////////////

AmbientOcclusionSettings& AmbientOcclusionPass::accessSettings()
{
   AmbientOcclusionSettings& settings = m_constantsBuf->accessData< AmbientOcclusionSettings >();
   return settings;
}

///////////////////////////////////////////////////////////////////////////////

void AmbientOcclusionPass::render( Renderer& renderer, RenderTarget2D* gBuffer, const Color& ambientColor, RenderTarget2D* outRenderTarget )
{
   const Shader* shader = m_shader.get();
   if ( !shader )
   {
      return;
   }

   // render
   MemoryAllocator* rcComm = renderer.rtComm();

   new ( rcComm ) RCActivateRenderTarget( m_intermediateAOTarget );

   AmbientOcclusionSettings& settings = m_constantsBuf->accessData< AmbientOcclusionSettings >();
   ShaderUtils::calculateHalfPixel( renderer, outRenderTarget, settings.m_halfPixel );
   settings.m_ambientColor = ambientColor;

   Camera& camera = renderer.getActiveCamera();
   const Matrix& projMtx = camera.getProjectionMtx();
   settings.m_projectionMtx = projMtx;
   settings.m_projParams.set( projMtx( 0, 0 ), projMtx( 1, 1 ), projMtx( 2, 2 ), projMtx( 3, 2 ) );

   RCBindShader* bindPS = new ( rcComm ) RCBindShader( shader, renderer );
   bindPS->setDepthBuffer( "g_DepthBuffer", gBuffer );
   bindPS->setTexture( "g_TexRandom", m_randomTexture, m_randomTextureSampler );
   bindPS->setTexture( "g_TexNormals", gBuffer, m_normalsBufferSampler, GBuf_Normals );
   bindPS->setTexture( "g_TexAlbedo", gBuffer, m_normalsBufferSampler, GBuf_Albedo );
   bindPS->setDataBuf( "Constants", m_constantsBuf );

   new ( rcComm ) RCFullscreenQuad( renderer.getViewportWidth(), renderer.getViewportHeight() );

   new ( rcComm ) RCUnbindShader();
   new ( rcComm ) RCDeactivateRenderTarget();

   // blur the image
   m_blurEffect->blur( renderer, m_intermediateAOTarget, outRenderTarget );
}

///////////////////////////////////////////////////////////////////////////////

void AmbientOcclusionPass::calculateKernel( uint size )
{
   size = min2<uint>( size, MAX_AO_KERNEL_SIZE );

   AmbientOcclusionSettings& settings = m_constantsBuf->accessData< AmbientOcclusionSettings >();
   settings.m_sampleKernelSize = size;

   for ( uint i = 0; i < size; ++i ) 
   {
      settings.m_kernel[i].set(
         randRange( -1.0f, 1.0 ),
         randRange( -1.0f, 1.0 ),
         randRange( 0.0f, 1.0 ) );

      settings.m_kernel[i].normalize();

      float scale = float( i ) / float( size );
      scale = lerp( 0.1f, 1.0f, scale * scale );
      settings.m_kernel[i].mul( FastFloat::fromFloat( scale ) );
   }
}

///////////////////////////////////////////////////////////////////////////////
