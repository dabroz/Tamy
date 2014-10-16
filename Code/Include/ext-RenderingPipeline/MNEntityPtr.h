/// @file   ext-RenderingPipeline\MNEntityPtr.h
/// @brief  node that relays a pointer to the host entity related data to the material system
#pragma once

#include "core-Renderer\MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSColorOutput;

///////////////////////////////////////////////////////////////////////////////

/**
* Node that relays a pointer to the host entity related data to the material system.
*/
class MNEntityPtr : public MaterialNode
{
   DECLARE_ALLOCATOR( MNEntityPtr, AM_DEFAULT );
   DECLARE_CLASS();

private:
   // runtime data
   MSColorOutput*                m_entityPtr;

   uint                          var_entityPtr;

public:
   /**
   * Constructor.
   *
   * @param name
   */
   MNEntityPtr( const char* name = "MNEntityPtr" );

   /**
   * Copy constructor.
   */
   MNEntityPtr( const MNEntityPtr& rhs );
   ~MNEntityPtr();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded() override;

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void buildMaterialShader( MaterialShaderBuilder& builder ) override;
   void render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const override;
};

///////////////////////////////////////////////////////////////////////////////
