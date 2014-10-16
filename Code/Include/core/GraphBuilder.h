/// @file   core/GraphBuilder.h
/// @brief  graph builder resource
#ifndef _GRAPH_BUILDER_H
#define _GRAPH_BUILDER_H

#include "core\Resource.h"
#include <vector>
#include "core\Graph.h"
#include "core\List.h"
#include "core\GraphBuilderListener.h"


///////////////////////////////////////////////////////////////////////////////

struct Vector;
class RuntimeDataBuffer;

template< typename NodeType >
class GraphBuilderTransaction;

template< typename NodeType >
class GBNodeInput;

template< typename NodeType >
class GBNodeInput;

///////////////////////////////////////////////////////////////////////////////

/**
 * Graph builder resource.
 */
template<typename NodeType >
class GraphBuilder
{
public:
   std::vector< NodeType* >*                    m_nodes;

private:
   typedef List< GraphBuilderListener< NodeType >* > ListenersList;
   ListenersList                                m_listeners;

public:
   /**
    * Constructor.
    */
   GraphBuilder();
   virtual ~GraphBuilder();

   /**
    * Sets an externally managed and serialized array of graph nodes
    *                   this builder will operate on
    *
    * @param nodes
    */
   void setGraphBuilderNodes( std::vector< NodeType* >* nodes );

   /**
    * Creates a copy of the specified selection of nodes.
    *
    * @param nodesInRegion
    * @param offset           offset by which all copied nodes will be translated
    */
   void copyRegion( const List< NodeType* >& nodes, const Vector& offset );

   /**
    * Creates a copy of the specified selection of nodes.
    *
    * @param nodesInRegion
    * @param offset           offset by which all copied nodes will be translated
    */
   void copyRegion( const std::vector< NodeType* >& nodes, const Vector& offset );

   /**
    * Clears the graph, deleting all nodes.
    *
    * This method will NOT inform the listeners about the operation. If you want to clear
    * the graph and have the listeners informed about it, use GraphBuilderTransaction::clear method
    */
   virtual void clear();

   /**
   * If some node classes are deleted, they will no longer be loaded, and the outputs that used to be connected
   * to the inputs of the existing nodes will hang in void.
   *
   * This method removes them.
   */
   void removeObsoleteSockets();

   /**
    * Finds a node of the specific type.
    */
   template< typename TSpecificNode >
   TSpecificNode* findNodeByType();

   // -------------------------------------------------------------------------
   // Graph instancing
   // -------------------------------------------------------------------------
   /**
    * Returns a list of all defined nodes.
    */
   inline const std::vector< NodeType* >& getNodes() const { return *m_nodes; }

   /**
    * Constructs a graph based on the definition.
    *
    * Edges start in outputs and end in inputs. The graph is built from every single node that doesn't have any inputs specified.
    *
    * @param outGraph      resulting graph
    */
   void buildGraph( Graph< NodeType* >& outGraph ) const;

   /**
    * Analyzes the specified nodes domain in search of the nodes
    * that precede the checkedNode in the graph ( whose outputs connect to that node's inputs ).
    *
    * @param checkedNode
    * @param inAllNodes       nodes we want to check
    * @param outPredecessors
    */
   void getPredecessors( NodeType* checkedNode, std::vector< NodeType* >& outPredecessors ) const;

   // -------------------------------------------------------------------------
   // Observers management
   // -------------------------------------------------------------------------

   /**
    * Attaches a new listener.
    *
    * @param listener
    */
   void attachGraphListener( GraphBuilderListener< NodeType >* listener );

   /**
    * Detaches a listener.
    *
    * @param listener
    */
   void detachGraphListener( GraphBuilderListener< NodeType >* listener );

   /**
    * Pushes the graph structure through to the specified listener.
    *
    * @param listener
    */
   void pullGraphStructure( GraphBuilderListener< NodeType >* listener );

   // -------------------------------------------------------------------------
   // GraphBuilderNodeOwner instancing
   // -------------------------------------------------------------------------
   void notifyConnectionBroken( const NodeType* endNode, const char* inputName ) const;
   
private:
   /**
    * A utility method that searches for a node who owns the specified output in the specified list of nodes.
    *
    * @param nodes
    * @param output
    */
   static NodeType* getParentNode( const List< NodeType* >& nodes, GBNodeOutput< NodeType >* output );
   
   /**
    * A utility method that searches for a node who owns the specified output in the specified list of nodes.
    *
    * @param nodes
    * @param output
    */
   static NodeType* getParentNode( const std::vector< NodeType* >& nodes, GBNodeOutput< NodeType >* output );

   // -------------------------------------------------------------------------
   // Transaction API
   // -------------------------------------------------------------------------
   friend class GraphBuilderTransaction< NodeType >;

   /**
    * Transaction adds a new node to the graph using this method.
    *
    * @param node       node to add
    */
   void addNode( NodeType* node );

   /**
    * Transaction removes a node from the graph using this method.
    *
    * @param node       node to remove
    */
   void removeNode( NodeType& node );

   /**
    * Establishes a connection between this node's output `outputName`,
    * and the `destNode's` input `inputName`
    *
    * @param outputName
    * @param destNode
    * @param inputName
    */
   void connect( NodeType* startNode, const std::string& outputName, NodeType* destNode, const std::string& inputName );

   /**
    * Disconnects this node from the specified node's input.
    *
    * @param destNode
    * @param inputName
    */
   void disconnect( NodeType* destNode, const std::string& inputName );

   /**
    * Disconnects this node from the specified node - disconnects all involved inputs.
    *
    * @param destNode
    */
   void disconnect( NodeType* startNode, NodeType* destNode );
};

///////////////////////////////////////////////////////////////////////////////

#include "core/GraphBuilder.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BUILDER_H
