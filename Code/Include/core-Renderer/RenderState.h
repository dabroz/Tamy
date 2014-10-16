/// @file   core-Renderer/RenderState.h
/// @brief  render state interface
#pragma once

#include "core\ReflectionObject.h"
#include "core-Renderer\MaterialNode.h"
#include "core\GraphBuilderListener.h"
#include "core\Stack.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class Material;
class MaterialShaderLayout;
class RuntimeDataBuffer;
class RCBindShader;
class GeometryComponent;
class ShaderParam;
class Shader;
class ShaderDataBuffer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Changes the rendering state of the graphics device.
 */
class RenderState : public ReflectionObject, public GraphBuilderListener< MaterialNode >
{
   DECLARE_ALLOCATOR( RenderState, AM_DEFAULT );
   DECLARE_CLASS()

public:
   // static data
   std::string                               m_name;
   Material*                                 m_material;

private:
   // runtime data
   bool                                      m_activeInRuntime;
   RuntimeDataBuffer*                        m_dataBuf;
   const GeometryComponent*                  m_parent;      // the state needs to keep track of the parent,
                                                            // because some of the material nodes need to be able to access
                                                            // it during rendering, and passing it along in the function calls
                                                            // would affect the performance
   Array< ShaderDataBuffer* >                m_routineConstantBuffers;

   ShaderDataBuffer*                         m_selectionPassDataBuffer;
   ShaderDataBuffer*                         m_selectionQueryPassDataBuffer;

   Shader*                                   m_singleColorShader;
   Shader*                                   m_selectionQueryShader;
   Shader*                                   m_shadowMapShader;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   RenderState( const char* name = "RenderState" );

   /**
    * Copy constructor.
    */
   RenderState( const RenderState& rhs );
   ~RenderState();

   /**
    * Returns the name of the render state.
    */
   const std::string& getName() const { return m_name; }
  
   /**
    * Called when it's time to initialize the state.
    *
    * @param parent
    */
   void onInitialize( const GeometryComponent* parent );

   /**
    * Called when it's time to deinitialize the state.
    */
   void onDeinitialize();

   /**
   * Sets a new material on the entity.
   *
   * @param material
   */
   void setMaterial( Material* material );

   /**
   * Sets the parent of the state.
   *
   * @param parent
   */
   inline const GeometryComponent* getParent() const {
      return m_parent;
   }

   // -------------------------------------------------------------------------
   // Rendering methods
   // -------------------------------------------------------------------------

   /**
    * Called before the geometry rendering - sets the render state on the device.
    *
    * @param renderer
    */
   RCBindShader* renderDefault( Renderer& renderer ) const;

   /**
    * Called to render the object for a selection query.
    *
    * @param renderer
    */
   RCBindShader* renderSelectionQuery( Renderer& renderer ) const;

   /**
    * Called to render the object as being selected in the editor.
    *
    * @param renderer
    */
   RCBindShader* renderSelection( Renderer& renderer ) const;

   /**
    * Called to render the object into a shadow map.
    *
    * @param renderer
    */
   RCBindShader* rendererShadowMap( Renderer& renderer ) const;

   /**
    * Called after the geometry rendering- resets the render state.
    *
    * @param renderer
    */
   void postRender( Renderer& renderer ) const;

   // -------------------------------------------------------------------------
   // Runtime data access
   // -------------------------------------------------------------------------

   /**
    * Returns the runtime data buffer.
    */
   inline RuntimeDataBuffer& data() const { return *m_dataBuf; }

   // -------------------------------------------------------------------------
   // States comparison
   // -------------------------------------------------------------------------
   inline static bool isEqual( const RenderState& rhs, const RenderState& lhs ) { return rhs.m_material == lhs.m_material; }
   inline static bool isLesser( RenderState* const& rhs, RenderState* const& lhs ) { return rhs->m_material < lhs->m_material; }
   inline static bool isGreater( RenderState* const& rhs, RenderState* const& lhs ) { return rhs->m_material > lhs->m_material; }

   // -------------------------------------------------------------------------
   // GraphBuilderListener< MaterialNode > implementation
   // -------------------------------------------------------------------------
   void onNodeCreated( MaterialNode* node );
   void onNodeRemoved( MaterialNode* node );

   // -------------------------------------------------------------------------
   // ReflectionObject implementation
   // -------------------------------------------------------------------------
   void onPrePropertyChanged( ReflectionProperty& property );
   void onPropertyChanged( ReflectionProperty& property );

private:
   void initializeMaterial();
   void deinitializeMaterial();
   void updateMaterialNodes( uint routineIdx, Renderer& renderer, RCBindShader* shaderComm ) const;
   void rebuildCustomShaders();
   void createShader( const Shader* defaultProfileShader, Shader*& shader, const char* postfix ) const;
};

///////////////////////////////////////////////////////////////////////////////
