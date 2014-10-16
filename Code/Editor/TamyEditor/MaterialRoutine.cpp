#include "MaterialRoutine.h"
#include "core-Renderer\MaterialNode.h"
#include "core-Renderer\MaterialProfile.h"
#include "core\GraphBuilder.h"
#include "core\GraphBuilderTransaction.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( MaterialRoutine, tmr, AM_BINARY )
   PROPERTY( std::vector< MaterialNode* >, m_nodes );
END_RESOURCE()

///////////////////////////////////////////////////////////////////////////////

MaterialRoutine::MaterialRoutine( const FilePath& path )
   : Resource( path )
{
}

///////////////////////////////////////////////////////////////////////////////

MaterialRoutine::~MaterialRoutine()
{
   const uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      MaterialNode* node = m_nodes[i];
      if ( node )
      {
         node->removeReference();
      }
   }
   m_nodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialRoutine::replaceContents( Resource& rhs )
{
   MaterialRoutine& rhsMaterialRoutine = static_cast< MaterialRoutine& >( rhs );
   m_nodes = rhsMaterialRoutine.m_nodes;

   // we need to clear the rhs resource's m_nodes instance, 'cause it will get deleted in just a sec
   // and we've just taken the ownership of them
   rhsMaterialRoutine.m_nodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void MaterialRoutine::grabSnapshot( const MaterialProfile* profile, uint routineIdx )
{
   const std::vector< MaterialNode* >& nodes = profile->getNodes( ( ShaderRoutine ) routineIdx );

   // delete old nodes
   const uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      MaterialNode* node = m_nodes[i];
      if ( node )
      {
         node->removeReference();
      }
   }
   m_nodes.clear();

   if ( !nodes.empty() )
   {
      // make a copy of the graph
      GraphBuilder< MaterialNode > copiedGraph;
      copiedGraph.setGraphBuilderNodes( &m_nodes );

      Vector worldToLocal;
      worldToLocal.setMul( nodes.front()->getPosition(), Float_Minus1 );
      copiedGraph.copyRegion( nodes, worldToLocal );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MaterialRoutine::pasteSnapshot( MaterialProfile* profile, uint routineIdx, const Vector& pastingOffset ) const
{
   GraphBuilder< MaterialNode >* graph = profile->getGraph( ( ShaderRoutine ) routineIdx );
   graph->copyRegion( m_nodes, pastingOffset );
}

///////////////////////////////////////////////////////////////////////////////
