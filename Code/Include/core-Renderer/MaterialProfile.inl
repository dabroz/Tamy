#ifndef _MATERIAL_PROFILE_H
#error "This file can only be included by MaterialProfile.h"
#else

#include "core-Renderer\MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

template< typename NodeType >
NodeType* MaterialProfile::findNodeByType( ShaderRoutine routine )
{
   std::vector< MaterialNode* >* nodesOfRoutine[] = {
      &m_vertexRoutineNodes,
      &m_tessControlRoutineNodes,
      &m_tessEvalRoutineNodes,
      &m_geometryRoutineNodes,
      &m_fragmentNodes
   };

   std::vector< MaterialNode* >& nodes = *nodesOfRoutine[routine];
   const uint count = nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      MaterialNode* node = nodes[i];
      if ( node->isA< NodeType >() )
      {
         return static_cast<NodeType*>( node );
      }
   }

   // no node of this type exists
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _MATERIAL_PROFILE_H
