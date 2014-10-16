/// @file   core-Renderer\MaterialProfile.h
/// @brief  a single profile that renders geometry to a specific context
#ifndef _MATERIAL_PROFILE_H
#define _MATERIAL_PROFILE_H

#include "core\ReflectionObject.h"
#include "core\GraphBuilder.h"
#include "core\GraphBuilderTransaction.h"
#include "core-Renderer\MaterialNode.h"
#include "core-Renderer\Shader.h"


///////////////////////////////////////////////////////////////////////////////

class RenderState;
class MaterialNode;
class Renderer;
class RuntimeDataBuffer;
class MaterialShaderLayout;
class Shader;

///////////////////////////////////////////////////////////////////////////////

class MaterialProfile : public ReflectionObject, public GraphBuilderListener< MaterialNode >
{
   DECLARE_ALLOCATOR( MaterialProfile, AM_DEFAULT );
   DECLARE_CLASS()

public:
   // runtime data
   Array< MaterialNode* >              m_nodesQueue[LAST_SHADER_ROUTINE];
   Shader*                             m_shader;

   GraphBuilder< MaterialNode >*       m_routineGraph[LAST_SHADER_ROUTINE];
   std::vector< MaterialNode* >*       m_nodes[LAST_SHADER_ROUTINE];

   IDString                            m_routineConstantBufferName[LAST_SHADER_ROUTINE];
   uint                                m_routineConstantBufferSize[LAST_SHADER_ROUTINE];

private:
   // static data
   std::vector< MaterialNode* >        m_vertexRoutineNodes;      // nodes for the VERTEX SHADER routine
   std::vector< MaterialNode* >        m_tessControlRoutineNodes; // nodes for the TESSELATION SHADER routine
   std::vector< MaterialNode* >        m_tessEvalRoutineNodes;    // nodes for the TESSELATION EVALUATION SHADER routine
   std::vector< MaterialNode* >        m_geometryRoutineNodes;    // nodes for the GEOMETRY SHADER routine
   std::vector< MaterialNode* >        m_fragmentNodes;           // nodes for the FRAGMENT SHADER routine

   bool                                m_transparent;
   bool                                m_doubleSided;

public:
   /**
    * Constructor.
    */
   MaterialProfile( );
   ~MaterialProfile();

   /**
    * Builds the profile.
    */
   void buildProfile();

   /**
    * Called by the parent Material resource when it gets loaded so that the profile can initialize
    * its runtime data.
    *
    * @param mgr
    * @param parentPath
    */
   void onResourceLoaded( ResourcesManager& mgr, const FilePath& parentPath );

   /**
    * Returns the shader.
    */
   inline const Shader* getShader() const {
      return m_shader;
   }

   /**
    * Returns a graph of the specified routine.
    *
    * @param routine
    */
   inline GraphBuilder< MaterialNode >* getGraph( ShaderRoutine routine ) {
      return m_routineGraph[routine];
   }

   /**
    * Returns the nodes that belong to the specific routine.
    *
    * @param routine
    */
   inline const std::vector< MaterialNode* >& getNodes( ShaderRoutine routine ) const {
      return *m_nodes[routine];
   }

   /**
   * Clears all subroutine graphs.
   */
   void clear();

   /**
    * Clears the specified subroutine's graph.
    *
    * @param routine
    */
   void clearRoutine( ShaderRoutine routine );

   /**
    * Finds a node of the specified type, or returns NULL, if none exists.
    *
    * @param routine
    */
   template< typename NodeType >
   NodeType* findNodeByType( ShaderRoutine routine );

   // -------------------------------------------------------------------------
   // Properties
   // -------------------------------------------------------------------------
   /**
    * Tells whether the profile should be rendered with the transparency enabled.
    */
   inline bool isTransparent() const {
      return m_transparent;
   }

   /**
   * Tells whether the object should be rendered as double sided.
   */
   inline bool isDoubleSided() const {
      return m_doubleSided;
   }

   // -------------------------------------------------------------------------
   // Listeners
   // -------------------------------------------------------------------------

   /**
    * Attaches a new listener.
    *
    * @param routine
    * @param listener
    */
   void attachGraphListener( ShaderRoutine routine, GraphBuilderListener< MaterialNode >* listener );

   /**
    * Detaches a listener.
    *
    * @param routine
    * @param listener
    */
   void detachGraphListener( ShaderRoutine routine, GraphBuilderListener< MaterialNode >* listener );

   /**
    * Pushes the graph structure through to the specified listener.
    *
    * @param routine
    * @param listener
    */
   void pullGraphStructure( ShaderRoutine routine, GraphBuilderListener< MaterialNode >* listener );

   // -------------------------------------------------------------------------
   // GraphBuilderListener< MaterialNode > implementation
   // -------------------------------------------------------------------------
   void onNodeRemoved( MaterialNode* node );
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\MaterialProfile.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _MATERIAL_PROFILE_H
