#ifndef _GRAPH_BUILDER_NODE_H
#error "This file can only be included from GraphBuilderNode.h"
#else

#include "core\Assert.h"
#include "core\Log.h"
#include "core\GraphBuilder.h"
#include "core\GraphBuilderSockets.h"
#include "core\GraphBuilderNodeUtils.h"
#include "core\ListUtils.h"
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
GraphBuilderNode< Impl >::GraphBuilderNode( const char* name )
   : m_name( name )
   , m_hostGraph( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
GraphBuilderNode< Impl >::GraphBuilderNode( const GraphBuilderNode& rhs )
   : ReflectionObject( rhs )
   , m_name( rhs.m_name )
   , m_hostGraph( NULL )
   , m_position( rhs.m_position )
{
   uint count = rhs.m_inputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      TInputSocket* input = rhs.m_inputs[i];

      const SerializableReflectionType& type = input->getVirtualRTTI();
      TInputSocket* inputCopy = type.instantiate( *input );
      m_inputs.push_back( inputCopy );
   }

   count = rhs.m_outputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      TOutputSocket* output = rhs.m_outputs[i];

      const SerializableReflectionType& type = output->getVirtualRTTI();
      TOutputSocket* outputCopy = type.instantiate( *output );
      m_outputs.push_back( outputCopy );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
GraphBuilderNode< Impl >::~GraphBuilderNode()
{
   // remove inputs and outputs
   for( InputsMap::iterator it = m_inputs.begin(); it != m_inputs.end(); ++it )
   {
      TInputSocket* input = *it;
      input->removeReference();
   }
   m_inputs.clear();

   for( OutputsMap::iterator it = m_outputs.begin(); it != m_outputs.end(); ++it )
   {
      TOutputSocket* output = *it;
      output->removeReference();
   }
   m_outputs.clear();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::setHost( const GraphBuilder< Impl >* hostGraph )
{
   m_hostGraph = hostGraph;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::attachNodeListener( GraphBuilderNodeListener< Impl >* listener )
{
   // check for duplicates
   ListenersList::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd() )
   {
      return;
   }

   m_listeners.pushBack( listener );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::detachNodeListener( GraphBuilderNodeListener< Impl >* listener )
{
   ListenersList::iterator it = ListUtils::find( m_listeners, listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::pullStructure( GraphBuilderNodeListener< Impl >* listener )
{
   uint count = m_inputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      TInputSocket* input = m_inputs[i];
      listener->onInputAdded( static_cast< Impl* >( this ), input );
   }

   count = m_outputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      TOutputSocket* output = m_outputs[i];
      listener->onOutputAdded( static_cast< Impl* >( this ), output );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::getSubsequentNodes( std::vector< Impl* >& outNodesToRun ) const
{
   for( OutputsMap::const_iterator it = m_outputs.begin(); it != m_outputs.end(); ++it )
   {
      const std::vector< Impl* >& connectedNodes = (*it)->getConnectedNodes();
      outNodesToRun.insert( outNodesToRun.end(), connectedNodes.begin(), connectedNodes.end() );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
bool GraphBuilderNode< Impl >::isConnectedWith( Impl* otherNode ) const
{
   uint outputsCount = m_outputs.size();
   for( uint outputIdx = 0; outputIdx < outputsCount; ++outputIdx )
   {
      const std::vector< Impl* >& connectedNodes = m_outputs[outputIdx]->getConnectedNodes();

      uint nodesCount = connectedNodes.size();
      for ( uint nodeIdx = 0; nodeIdx < nodesCount; ++nodeIdx )
      {
         if ( connectedNodes[nodeIdx] == otherNode )
         {
            // found it - there is a connection between the nodes.
            return true;
         }
      }
   }

   // the two nodes are not connected
   return false;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
GBNodeInput< Impl >* GraphBuilderNode< Impl >::findInput( const std::string& inputName ) const
{
   for ( InputsMap::const_iterator it = m_inputs.begin(); it != m_inputs.end(); ++it )
   {
      if ( (*it)->getName() == inputName )
      {
         return *it;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
GBNodeOutput< Impl >* GraphBuilderNode< Impl >::findOutput( const std::string& outputName ) const
{
   for ( OutputsMap::const_iterator it = m_outputs.begin(); it != m_outputs.end(); ++it )
   {
      if ( (*it)->getName() == outputName )
      {
         return *it;
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::defineInput( TInputSocket* input )
{
   TInputSocket* existingInput = findInput( input->getName() );
   if ( existingInput )
   {
      char tmp[128];
      sprintf_s( tmp, "Trying to override the definition of input '%s'", existingInput->getName().c_str() );
      LOG( tmp );

      delete input;
      return;
   }


   m_inputs.push_back( input );

   // notify listeners
   for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderNodeListener< Impl >* listener = *it;
      listener->onInputAdded( static_cast< Impl* >( this ), input );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::defineInputs( const std::vector< TInputSocket* >& inputs )
{
   uint count = inputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      TInputSocket* newInput = inputs[i];
      defineInput( newInput );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::removeAllInputs()
{
   if ( m_inputs.empty() )
   {
      // there's nothing to be removed here
      return;
   }

   uint count = m_inputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      TInputSocket* input = m_inputs[i];
      
      // remove all connections that involve this input
      removeConnections( input );

      // notify listeners
      for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
      {
         GraphBuilderNodeListener< Impl >* listener = *it;
         listener->onInputRemoved( static_cast< Impl* >( this ), input );
      }

      delete input;
   }
   m_inputs.clear();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::removeSelectedInputs( const std::vector< std::string >& inputNames )
{
   if ( m_inputs.empty() || inputNames.empty() )
   {
      // there's nothing to be removed here
      return;
   }

   uint namesCount = inputNames.size();
   for ( uint nameIdx = 0; nameIdx < namesCount; ++nameIdx )
   {
      const std::string& removedInputName = inputNames[nameIdx];

      uint count = m_inputs.size();
      for ( uint i = 0; i < count; ++i )
      {
         if ( m_inputs[i]->getName() == removedInputName )
         {
            TInputSocket* input = m_inputs[i];

            // remove all connections that involve this input
            removeConnections( input );

            // notify listeners
            for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
            {
               GraphBuilderNodeListener< Impl >* listener = *it;
               listener->onInputRemoved( static_cast< Impl* >( this ), input );
            }

            // remove the input
            delete input;
            m_inputs.erase( m_inputs.begin() + i );
            break;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::redefineInputs( const std::vector< TInputSocket* >& newInputs )
{
   List< uint > socketsToRemove;
   List< uint > socketsToAdd;
   GraphBuilderNodeUtils::mergeSocketsLists( newInputs, m_inputs, socketsToRemove, socketsToAdd );

   // remove sockets
   for ( List< uint >::iterator it = socketsToRemove.end(); !it.isEnd(); --it )
   {
      uint inputIdx = *it;
      TInputSocket* input = m_inputs[inputIdx];

      // remove all connections that involve this input
      removeConnections( input );

      // notify listeners
      for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
      {
         GraphBuilderNodeListener< Impl >* listener = *it;
         listener->onInputRemoved( static_cast< Impl* >( this ), input );
      }

      delete input;
      m_inputs.erase( m_inputs.begin() + inputIdx );
   }

   // add sockets
   for ( List< uint >::iterator it = socketsToAdd.begin(); !it.isEnd(); ++it )
   {
      uint inputIdx = *it;
      TInputSocket* input = newInputs[inputIdx];

      // notify listeners
      for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
      {
         GraphBuilderNodeListener< Impl >* listener = *it;
         listener->onInputAdded( static_cast< Impl* >( this ), input );
      }

      m_inputs.push_back( input );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::defineOutput( TOutputSocket* output )
{
   // make sure we're not overriding the output
   TOutputSocket* existingOutput = findOutput( output->getName() );
   if ( existingOutput )
   {
      char tmp[128];
      sprintf_s( tmp, "Trying to override the definition of output '%s'", existingOutput->getName().c_str() );
      LOG( tmp );

      delete output;
      return;
   }

   m_outputs.push_back( output );

   // notify listeners
   for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderNodeListener< Impl >* listener = *it;
      listener->onOutputAdded( static_cast< Impl* >( this ), output );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::defineOutputs( const std::vector< TOutputSocket* >& outputs )
{
   uint outputsAdded = 0;

   uint count = outputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      TOutputSocket* newOutput = outputs[i];
      defineOutput( newOutput );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::removeAllOutputs()
{
   if ( m_outputs.empty() )
   {
      // there's nothing to be removed here
      return;
   }

   uint count = m_outputs.size();
   for ( uint i = 0; i < count; ++i )
   {
      TOutputSocket* output = m_outputs[i];

      // remove all connections that involve this output
      removeConnections( output );

      // notify listeners
      for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
      {
         GraphBuilderNodeListener< Impl >* listener = *it;
         listener->onOutputRemoved( static_cast< Impl* >( this ), output );
      }

      delete output;
   }
   m_outputs.clear();

}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::removeSelectedOutputs( const std::vector< std::string >& outputNames )
{
   if ( m_outputs.empty() || outputNames.empty() )
   {
      // there's nothing to be removed here
      return;
   }

   uint namesCount = outputNames.size();
   for ( uint nameIdx = 0; nameIdx < namesCount; ++nameIdx )
   {
      const std::string& removedOutputName = outputNames[nameIdx];

      uint count = m_outputs.size();
      for ( uint i = 0; i < count; ++i )
      {
         if ( m_outputs[i]->getName() == removedOutputName )
         {
            TOutputSocket* output = m_outputs[i];

            // remove all connections that involve this input
            removeConnections( output );

            // notify listeners
            for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
            {
               GraphBuilderNodeListener< Impl >* listener = *it;
               listener->onOutputRemoved( static_cast< Impl* >( this ), output );
            }

            // remove the output
            delete output;
            m_outputs.erase( m_outputs.begin() + i );
            break;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::redefineOutputs( const std::vector< TOutputSocket* >& newOutputs )
{
   List< uint > socketsToRemove;
   List< uint > socketsToAdd;
   GraphBuilderNodeUtils::mergeSocketsLists( newOutputs, m_outputs, socketsToRemove, socketsToAdd );

   // remove sockets
   for ( List< uint >::iterator it = socketsToRemove.end(); !it.isEnd(); --it )
   {
      uint outputIdx = *it;
      TOutputSocket* output = m_outputs[outputIdx];

      // remove all connections that involve this input
      removeConnections( output );

      // notify listeners
      for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
      {
         GraphBuilderNodeListener< Impl >* listener = *it;
         listener->onOutputRemoved( static_cast< Impl* >( this ), output );
      }

      delete output;
      m_inputs.erase( m_inputs.begin() + outputIdx );
   }

   // add sockets
   for ( List< uint >::iterator it = socketsToAdd.begin(); !it.isEnd(); ++it )
   {
      uint outputIdx = *it;
      TOutputSocket* output = newOutputs[outputIdx];

      // notify listeners
      for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
      {
         GraphBuilderNodeListener< Impl >* listener = *it;
         listener->onOutputAdded( static_cast< Impl* >( this ), output );
      }

      m_outputs.push_back( output );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl > template< typename T >
const T& GraphBuilderNode< Impl >::getInput( const std::string& inputName ) const
{
   TInputSocket* input = findInput( inputName );
   if ( input == NULL)
   {
      char tmp[128];
      sprintf_s( tmp, "Unable to find input '%s'", inputName.c_str() );
      ASSERT_MSG( false, tmp );
   }

   T* dataSource = DynamicCast< T >( input );
   if ( dataSource == NULL )
   {
      char tmp[128];
      sprintf_s( tmp, "Input's '%s' type doesn't match the specified one", inputName.c_str() );
      ASSERT_MSG( false, tmp );
   }

   return *dataSource;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl > template< typename T >
const T& GraphBuilderNode< Impl >::getOutput( const std::string& outputName ) const
{
   TOutputSocket* output = findOutput( outputName );
   if ( output == NULL )
   {
      char tmp[128];
      sprintf_s( tmp, "Unable to find output '%s'", outputName.c_str() );
      ASSERT_MSG( false, tmp );
   }

   T* castOutput = DynamicCast< T >( output );
   if ( castOutput == NULL )
   {
      char tmp[128];
      sprintf_s( tmp, "Output's '%s' type doesn't match the specified one", outputName.c_str() );
      ASSERT_MSG( false, tmp );
   }

   return *castOutput;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::removeConnections( TInputSocket* input ) const
{
   ASSERT( m_hostGraph );

   const Impl* impl = static_cast< const typename Impl* >( this );
   m_hostGraph->notifyConnectionBroken( impl, input->getName().c_str() );

   input->disconnect();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::removeConnections( TOutputSocket* output ) const
{
   ASSERT( m_hostGraph );

   const std::vector< Impl* >& connectedNodes = output->getConnectedNodes();
   uint nodesCount = connectedNodes.size();
   for ( uint nodeIdx = 0; nodeIdx < nodesCount; ++nodeIdx )
   {
      Impl* connectedNode = connectedNodes[nodeIdx];
      if ( !connectedNode )
      {
         continue;
      }

      const std::vector< TInputSocket* >& inputs = connectedNode->getInputs();
      uint inputsCount = inputs.size();
      for ( uint inputIdx = 0; inputIdx < inputsCount; ++inputIdx )
      {
         TInputSocket* input = inputs[inputIdx];
         if ( input )
         {
            m_hostGraph->notifyConnectionBroken( connectedNode, input->getName().c_str() );
            input->disconnect();
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Impl >
void GraphBuilderNode< Impl >::setPosition( const Vector& pos )
{
   m_position = pos;

   Impl* nodeImpl = static_cast< Impl* >( this );

   // notify listeners
   for ( ListenersList::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      GraphBuilderNodeListener< Impl >* listener = *it;
      listener->onNodeAppearance( nodeImpl );
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BUILDER_NODE_H
