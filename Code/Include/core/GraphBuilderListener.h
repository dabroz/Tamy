/// @file   core/GraphBuilderListener.h
/// @brief  graph builder listener interface
#pragma once


///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
class GBNodeOutput;

template< typename TNode >
class GBNodeInput;

///////////////////////////////////////////////////////////////////////////////

/**
 * Graph builder listener interface.
 */
template< typename TNode >
class GraphBuilderListener
{
public:
   virtual ~GraphBuilderListener() {}

   /**
    * Called when a new node was created.
    *
    * @param node
    */
   virtual void onNodeCreated( TNode* node ) {}

   /**
    * Called when a node is about to be removed.
    *
    * @param node
    */
   virtual void onNodeRemoved( TNode* node ) {}

   /**
    * Called after a node has been removed.
    *
    * @param node
    */
   virtual void afterNodeRemoved( TNode* node ) {}

   /**
    * Called when two nodes were connected with each other.
    *
    * @param startNode
    * @param outputName
    * @param endNode
    * @param inputName
    */
   virtual void onConnectionCreated( const TNode* startNode, const char* outputName, const TNode* endNode, const char* inputName ) {}

   /**
    * Called when a connection between two nodes is being broken.
    *
    * @param endNode
    * @param inputName
    */
   virtual void onConnectionRemoved( const TNode* endNode, const char* inputName ) {}

   /**
    * Called after a connection between two nodes has been broken.
    *
    * @param endNode
    * @param inputName
    */
   virtual void afterConnectionRemoved( const TNode* endNode, const char* inputName ) {}
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Graph builder node listener interface.
 */
template< typename TNode >
class GraphBuilderNodeListener
{
public:
   virtual ~GraphBuilderNodeListener() {}

   /**
    * Called when something about a node changes.
    *
    * @param node
    */
   virtual void onNodeAppearance( TNode* node ) {}

   /**
    * Called when a new input socket was added.
    *
    * @param node
    * @param input
    */
   virtual void onInputAdded( TNode* node, GBNodeInput< TNode >* input ) {}

   /**
    * Called when a new output socket was added.
    *
    * @param node
    * @param output
    */
   virtual void onOutputAdded( TNode* node, GBNodeOutput< TNode >* output ) {}

   /**
    * Called when an input socket was removed.
    *
    * @param node
    * @param input
    */
   virtual void onInputRemoved( TNode* node, GBNodeInput< TNode >* input ) {}

   /**
    * Called when an output socket was removed.
    *
    * @param node
    * @param output
    */
   virtual void onOutputRemoved( TNode* node, GBNodeOutput< TNode >* output ) {}
};

///////////////////////////////////////////////////////////////////////////////
