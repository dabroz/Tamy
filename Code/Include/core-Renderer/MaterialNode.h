/// @file   core-Renderer/MaterialNode.h
/// @brief  material graph node
#pragma once

#include "core\Resource.h"
#include "core\GraphBuilderNode.h"
#include "core-Renderer\MaterialSockets.h"
#include "core-Renderer\Defines.h"


///////////////////////////////////////////////////////////////////////////////

class RenderState;
class GeometryComponent;
class Renderer;
class MaterialShaderBuilder;
class RCBindShader;
class ShaderDataBuffer;

///////////////////////////////////////////////////////////////////////////////

class MaterialNode : public GraphBuilderNode< MaterialNode >
{
   DECLARE_ALLOCATOR( MaterialNode, AM_ALIGNED_16 );
   DECLARE_CLASS()

public:
   /**
    * Constructor.
    *
    * @param name
    */
   MaterialNode( const char* name = "MaterialNode" );

   /**
    * Copy constructor.
    *
    * @param rhs
    */
   MaterialNode( const MaterialNode& rhs );
   virtual ~MaterialNode();

   /**
    * Creates an input with the specified name of the specified type.
    *
    * @param dataType      what type of data should the input marshal
    * @param name          input name
    */
   GBNodeInput< MaterialNode >* createInput( const ReflectionType& dataType, const std::string& name ) const;

   /**
    * Called in order to initialize node's runtime data layout.
    *
    * @param host
    */
   void createLayout( const RenderState& host ) const;

   /**
    * Called in order to deinitialize node's runtime data layout.
    *
    * @param host
    */
   void destroyLayout( const RenderState& host ) const;

   /**
    * Called when the graph is fully restored. Allows to reprocess certain nodes - remove
    * unused sockets etc. and get the graph structure up to date with external
    * dependencies in general.
    */
   virtual void onGraphLoaded() {}

   /**
    * The node can contribute here to the built pixel shader's code and settings.
    */
   virtual void buildMaterialShader( MaterialShaderBuilder& builder ) {}

   /**
    * Called by the instance that uses the material in order to set it up.
    *
    * @param renderer
    * @param instance      rendered material instance
    * @param shaderComm
    * @param constantsBuf
    */
   virtual void render( Renderer& renderer, const RenderState& host, RCBindShader* shaderComm, ShaderDataBuffer* constantsBuf ) const {}

protected:
   /**
    * Called in order to initialize node implementation's specific runtime data layout.
    *
    * @param host
    */
   virtual void onCreateLayout( const RenderState& host ) const {}

   /**
    * Called in order to initialize node implementation's specific runtime data layout.
    *
    * @param host
    */
   virtual void onDestroyLayout( const RenderState& host ) const {}
};

///////////////////////////////////////////////////////////////////////////////
