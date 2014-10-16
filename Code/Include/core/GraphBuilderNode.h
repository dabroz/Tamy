/// @file   core/GraphBuilderNode.h
/// @brief  basic graph builder node
#ifndef _GRAPH_BUILDER_NODE_H
#define _GRAPH_BUILDER_NODE_H

#include "core\ReflectionObject.h"
#include "core\GraphBuilderSockets.h"
#include "core\GraphBuilderListener.h"
#include "core\List.h"
#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

class GraphInstanceOwner;

template< typename Impl >
class GraphBuilder;

///////////////////////////////////////////////////////////////////////////////

/**
 * A graph builder node.
 *
 * A node keeps track of the nodes it's connected to ( for speed access
 * reasons ).
 *
 * There are two major socket types: input and output, and they follow these rules:
 * 1.) an output socket may have an infinite number of outgoing connections
 * 2.) an input socket may have only one incoming connection
 * 3.) an output socket has a type and is a data sink for the host node.
 *     If a node wants to store it's processing results for other nodes to
 *     use - an output socket is a place for that
 * 4.) an input socket is a data proxy - it takes data from an output socket
 *     it's connected to. Thus a strong typing is required here.
 * 5.) After an implementation-specific update takes place, the node
 *     schedules all outgoing connections for further processing with the host mechanism.
 */
template< typename Impl >
class GraphBuilderNode : public ReflectionObject
{
   DECLARE_ALLOCATOR( GraphBuilderNode, AM_ALIGNED_16 );

protected:
   typedef GBNodeInput< Impl > TInputSocket;
   typedef GBNodeOutput< Impl > TOutputSocket;
   typedef std::vector< TInputSocket* >         InputsMap;
   typedef std::vector< TOutputSocket* >        OutputsMap;

protected:
   std::string                                  m_name;
   InputsMap                                    m_inputs;
   OutputsMap                                   m_outputs;
   Vector                                       m_position;

private:
   const GraphBuilder< Impl >*                  m_hostGraph;

private:
   typedef List< GraphBuilderNodeListener< Impl >* > ListenersList;
   ListenersList                                m_listeners;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   GraphBuilderNode( const char* name = "" );
   
   /**
    * Copy constructor.
    *
    * @param rhs
    */
   GraphBuilderNode( const GraphBuilderNode& rhs );
   virtual ~GraphBuilderNode();

   /**
    * Sets a pointer to the graph instance that owns this node.
    *
    * @param hostGraph
    */
   void setHost( const GraphBuilder< Impl >* hostGraph );

   /**
    * Gives the node another name.
    *
    * @param name
    */
   inline void setName( const char* name ) {
      m_name = name;
   }

   /**
    * Returns the name of the node.
    */
   inline const std::string& getName() const {
      return m_name;
   }

   /**
    * Returns nodes that should be run after this node.
    *
    * @param outNodesToRun    an array to append the nodes to
    */
   void getSubsequentNodes( std::vector< Impl* >& outNodesToRun ) const;

   // -------------------------------------------------------------------------
   // Observers management
   // -------------------------------------------------------------------------

   /**
    * Attaches a new listener.
    *
    * @param listener
    */
   void attachNodeListener( GraphBuilderNodeListener< Impl >* listener );

   /**
    * Detaches a listener.
    *
    * @param listener
    */
   void detachNodeListener( GraphBuilderNodeListener< Impl >* listener );

   /**
    * Pushes the node structure through to the specified listener.
    *
    * @param listener
    */
   void pullStructure( GraphBuilderNodeListener< Impl >* listener );

   // -------------------------------------------------------------------------
   // Connections management
   // -------------------------------------------------------------------------
   
   /**
    * Returns an array of all defined input sockets.
    */
   inline const std::vector< TInputSocket* >& getInputs() const { return m_inputs; }

   /**
    * Returns an array of all defined output sockets.
    */
   inline const std::vector< TOutputSocket* >& getOutputs() const { return m_outputs; }

   /**
    * Looks for the specified input, or returns NULL if one can't be found.
    *
    * NOTE: even though the method returns a pointer, the caller is not supposed to destroy
    *       the returned object!
    *
    * @param inputName
    */
   TInputSocket* findInput( const std::string& inputName ) const;

   /**
    * Looks for the specified output, or returns NULL if one can't be found.
    *
    * NOTE: even though the method returns a pointer, the caller is not supposed to destroy
    *       the returned object!
    *
    * @param outputName
    */
   TOutputSocket* findOutput( const std::string& outputName ) const;

   /**
    * Checks if this node is directly connected with the specified node.
    *
    * @param otherNode
    */
   bool isConnectedWith( Impl* otherNode ) const;

   // -------------------------------------------------------------------------
   // Sockets management
   // -------------------------------------------------------------------------
   /**
    * Defines a single node input. 
    *
    * Don't use it if you define multiple inputs at once, as it'll generate a lot of update traffic.
    * Use 'defineInputs' instead.
    *
    * @param input
    */
   void defineInput( TInputSocket* input );

   /**
    * Defines node's inputs.
    *
    * @param inputs
    */
   void defineInputs( const std::vector< TInputSocket* >& inputs );

   /**
    * Re-defines node's inputs, removing the existing ones ( or preserving them if the supplied list
    * of inputs contains some that have the same name as the existing ones ), and creating new ones.
    *
    * Very useful if you would have to call removeAllInputs and then defineInputs, as it will
    * save you unnecessary update traffic.
    *
    * @param input
    */
   void redefineInputs( const std::vector< TInputSocket* >& inputs );

   /**
    * Removes all of node's inputs.
    */
   void removeAllInputs();

   /**
    * Removes selected inputs.
    *
    * @param inputNames
    */
   void removeSelectedInputs( const std::vector< std::string >& inputNames );

   /**
    * Defines a single node output. 
    *
    * Don't use it if you define multiple outputs at once, as it'll generate a lot of update traffic.
    * Use 'defineInputs' instead.
    *
    * @param output
    */
   void defineOutput( TOutputSocket* output );

   /**
    * Defines node's outputs.
    *
    * @param outputs
    */
   void defineOutputs( const std::vector< TOutputSocket* >& outputs );

   /**
    * Re-defines node's outputs, removing the existing ones ( or preserving them if the supplied list
    * of inputs contains some that have the same name as the existing ones ), and creating new ones.
    *
    * Very useful if you would have to call removeAllOutputs and then defineOutputs, as it will
    * save you unnecessary update traffic.
    *
    * @param outputs
    */
   void redefineOutputs( const std::vector< TOutputSocket* >& outputs );

   /**
    * Removes all of node's outputs.
    */
   void removeAllOutputs();

   /**
    * Removes selected outputs.
    *
    * @param outputNames
    */
   void removeSelectedOutputs( const std::vector< std::string >& outputNames );

   /**
    * Returns the specified input.
    *
    * @param T             node input type
    * @param inputName
    */
   template< typename T >
   const T& getInput( const std::string& inputName ) const;

   /**
    * Returns the specified output.
    *
    * @param T             node output type
    * @param outputName
    */
   template< typename T >
   const T& getOutput( const std::string& outputName ) const;

   // -------------------------------------------------------------------------
   // Appearance management
   // -------------------------------------------------------------------------
   /**
    * Sets new position of the node in the visual graph.
    *
    * @param pos
    */
   void setPosition( const Vector& pos );

   /**
    * Returns position of the node in the visual graph.
    */
   inline const Vector& getPosition() const { return m_position; }

private:
   void removeConnections( TInputSocket* input ) const;
   void removeConnections( TOutputSocket* output ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core/GraphBuilderNode.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BUILDER_NODE_H
