/// @file   core-Renderer/MNTimer.h
/// @brief  a node that provides current time value
#pragma once

#include "core-Renderer/MaterialNode.h"
#include "core/Timer.h"


///////////////////////////////////////////////////////////////////////////////

class MSFloatOutput;

///////////////////////////////////////////////////////////////////////////////

class MNTimer : public MaterialNode
{
   DECLARE_ALLOCATOR( MNTimer, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   float             m_period;
   float             m_amplitude;

   // runtime data
   MSFloatOutput*    m_time;
   MSFloatOutput*    m_sinTime;
   CTimer            m_timer;

   uint              var_time;
   uint              var_sinTime;
 
public:
   /**
    * Constructor.
    */
   MNTimer();

   /**
    * Copy constructor.
    */
   MNTimer( const MNTimer& rhs );
   ~MNTimer();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded() override;

   // -------------------------------------------------------------------------
   // MaterialNode implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
   void render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const override;
};

///////////////////////////////////////////////////////////////////////////////
