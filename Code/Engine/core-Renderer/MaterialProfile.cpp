#include "core-Renderer\MaterialProfile.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\MaterialNode.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\MaterialShaderBuilder.h"
#include "core-Renderer\Shader.h"
#include "core\RuntimeData.h"
#include "core\GraphUtils.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MaterialProfile );
   PARENT( ReflectionObject );
   PROPERTY( std::vector< MaterialNode* >, m_vertexRoutineNodes );
   PROPERTY( std::vector< MaterialNode* >, m_tessControlRoutineNodes );
   PROPERTY( std::vector< MaterialNode* >, m_tessEvalRoutineNodes );
   PROPERTY( std::vector< MaterialNode* >, m_geometryRoutineNodes );
   PROPERTY( std::vector< MaterialNode* >, m_fragmentNodes );
   PROPERTY_EDIT( "Transparent", bool, m_transparent );
   PROPERTY_EDIT( "Double sided", bool, m_doubleSided );
END_RESOURCE()

///////////////////////////////////////////////////////////////////////////////

MaterialProfile::MaterialProfile()
   : m_shader( new Shader() )
   , m_transparent( false )
   , m_doubleSided( false )
{
   m_nodes[0] = &m_vertexRoutineNodes;
   m_nodes[1] = &m_tessControlRoutineNodes;
   m_nodes[2] = &m_tessEvalRoutineNodes;
   m_nodes[3] = &m_geometryRoutineNodes;
   m_nodes[4] = &m_fragmentNodes;

   for ( uint i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      m_routineGraph[i] = new GraphBuilder< MaterialNode >();
      m_routineGraph[i]->setGraphBuilderNodes( m_nodes[i] );
   }

   memset( m_routineConstantBufferSize, 0, sizeof( uint ) * LAST_SHADER_ROUTINE );
}

///////////////////////////////////////////////////////////////////////////////

MaterialProfile::~MaterialProfile()
{
   for ( uint i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      std::vector< MaterialNode* >* nodes = m_nodes[i];

      const uint count = nodes->size();
      for ( uint j = 0; j < count; ++j )
      {
         MaterialNode* node = ( *nodes )[j];
         if ( node )
         {
            node->removeReference();
         }
      }
      nodes->clear();
   }

   // delete the graphs
   for ( uint i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      delete m_routineGraph[i];
      m_routineGraph[i] = NULL;
   }

   if ( m_shader )
   {
      m_shader->removeReference();
      m_shader = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void MaterialProfile::clear()
{
   for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
   {
      clearRoutine( ( ShaderRoutine)routineIdx );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MaterialProfile::clearRoutine( ShaderRoutine routine )
{
   m_nodesQueue[routine].clear();

   GraphBuilderTransaction< MaterialNode > transaction( *m_routineGraph[routine] );
   transaction.clearGraph();
   transaction.commit();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialProfile::onResourceLoaded( ResourcesManager& mgr, const FilePath& parentPath )
{
   // initialize nodes
   for ( uint i = 0; i < LAST_SHADER_ROUTINE; ++i )
   {
      std::vector< MaterialNode* >* nodes = m_nodes[i];

      bool anyNodesRemoved = false;
      int count = nodes->size();
      for ( int j = count - 1; j >= 0; --j )
      {
         MaterialNode* node = ( *nodes )[j];
         if ( node )
         {
            node->onGraphLoaded();
         }
         else
         {
            // remove obsolete node
            anyNodesRemoved = true;
            nodes->erase( nodes->begin() + j );
         }
      }

      if ( anyNodesRemoved )
      {
         m_routineGraph[i]->removeObsoleteSockets();
      }

      m_routineGraph[i]->setGraphBuilderNodes( nodes );

      // start listening to the graph
      m_routineGraph[i]->attachGraphListener( this );
   }

   // set the shader path - if saved, it should end up in the same directory, right next to the material resource
   {
      std::string fileNameTrunk;
      {
         // strip the path of the extension
         fileNameTrunk = parentPath.getRelativePath();
         std::size_t pos = fileNameTrunk.find_last_of( "." );
         if ( pos != std::string::npos )
         {
            fileNameTrunk = fileNameTrunk.substr( 0, pos );
         }
      }

      char tmpPath[512];
      sprintf_s( tmpPath, "%s.code", fileNameTrunk.c_str() );
      m_shader->setPath( FilePath( tmpPath ) );
   }

   // rebuild the shader
   buildProfile();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialProfile::buildProfile()
{
   for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
   {
      GraphBuilder< MaterialNode >* graphBuilder = m_routineGraph[routineIdx];
      Array< MaterialNode* >& nodesQueue = m_nodesQueue[routineIdx];

      // linearize nodes
      {
         nodesQueue.clear();

         // create the material graph
         Graph< MaterialNode* > materialGraph;
         graphBuilder->buildGraph( materialGraph );

         Array< int > sortedNodes;
         GraphUtils< MaterialNode* >::topologicalSort( sortedNodes, materialGraph );

         // insert the nodes backwards, because the graph was built backwards, but we still want
         // to run the update from the start nodes to the end node
         uint count = sortedNodes.size();
         for ( uint i = 0; i < count; ++i )
         {
            int idx = sortedNodes[i];
            nodesQueue.push_back( materialGraph.getNode( idx ) );
         }
      }

      // build shaders
      MaterialShaderBuilder shaderBuilder( ( ShaderRoutine ) routineIdx );
      uint count = nodesQueue.size();
      for ( uint i = 0; i < count; ++i )
      {
         MaterialNode* node = nodesQueue[i];
         node->buildMaterialShader( shaderBuilder );
      }

      std::string constantBufferName;
      shaderBuilder.compile( m_shader, m_routineConstantBufferSize[routineIdx], constantBufferName );
      m_routineConstantBufferName[routineIdx] = constantBufferName;
   }

   // rebuild the shader
   m_shader->build();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialProfile::attachGraphListener( ShaderRoutine routine, GraphBuilderListener< MaterialNode >* listener )
{
   m_routineGraph[routine]->attachGraphListener( listener );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialProfile::detachGraphListener( ShaderRoutine routine, GraphBuilderListener< MaterialNode >* listener )
{
   m_routineGraph[routine]->detachGraphListener( listener );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialProfile::pullGraphStructure( ShaderRoutine routine, GraphBuilderListener< MaterialNode >* listener )
{
   m_routineGraph[routine]->pullGraphStructure( listener );
}

///////////////////////////////////////////////////////////////////////////////

void MaterialProfile::onNodeRemoved( MaterialNode* node )
{
   // remove the node from the queues
   for ( uint routineIdx = 0; routineIdx < LAST_SHADER_ROUTINE; ++routineIdx )
   {
      Array< MaterialNode* >& nodesQueue = m_nodesQueue[routineIdx];
      const uint nodesCount = nodesQueue.size();
      for ( uint nodeIdx = 0; nodeIdx < nodesCount; ++nodeIdx )
      {
         if ( nodesQueue[nodeIdx] == node )
         {
            nodesQueue.remove( nodeIdx );
            return;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
