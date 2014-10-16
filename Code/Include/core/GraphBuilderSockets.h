/// @file   core/GraphBuilderSockets.h
/// @brief  graph builder node base sockets definitions.
#ifndef _GRAPH_BUILDER_SOCKETS_H
#define _GRAPH_BUILDER_SOCKETS_H

#include "core\ReflectionObject.h"
#include "core\Subject.h"
#include "core\RuntimeData.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class GraphInstanceOwner;

///////////////////////////////////////////////////////////////////////////////

enum GBNodeSocketOperation
{
   GBNSO_CHANGED
};

///////////////////////////////////////////////////////////////////////////////

class GBNodeSocket : public ReflectionObject, public Subject< GBNodeSocket, GBNodeSocketOperation >
{
   DECLARE_ALLOCATOR( GBNodeSocket, AM_ALIGNED_16 );

protected:
   // static data
   std::string                               m_name;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   GBNodeSocket( const std::string& name = "" );

   /**
    * Copy constructor.
    */
   GBNodeSocket( const GBNodeSocket& rhs );

   /**
    * Returns the socket's name.
    */
   inline const std::string& getName() const { return m_name; }

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged( ReflectionProperty& property );
};

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
class GBNodeInput;

/**
 * Base class for an output socket for a specific type of node.
 */
template< typename TNode >
class GBNodeOutput : public GBNodeSocket
{
   DECLARE_ALLOCATOR( GBNodeOutput, AM_ALIGNED_16 );

protected:
   std::vector< TNode* >     m_connectedNodes;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   GBNodeOutput( const std::string& name = "" );

   /**
    * Copy constructor.
    */
   GBNodeOutput( const GBNodeOutput& rhs );
   virtual ~GBNodeOutput();

   /**
    * Returns an array of nodes connected to this socket.
    */
   const std::vector< TNode* >& getConnectedNodes() const { return m_connectedNodes; }

   /**
    * Collects all connected nodes, appending them to the specified list.
    *
    * @param outNodes
    */
   void collectConnectedNodes( List< TNode* >& outNodes );

   /**
    * Connects the socket to the specified node.
    *
    * @param node
    */
   void connect( TNode& node );

   /**
    * Disconnects the socket from the specified node.
    *
    * @param node
    */
   void disconnect( TNode& node );

   /**
    * Called in order to initialize runtime data.
    *
    * @param data    runtime data buffer
    */
   virtual void createLayout( RuntimeDataBuffer& data ) const {}

   /**
    * Called in order to deinitialize runtime data.
    *
    * @param data    runtime data buffer
    */
   virtual void destroyLayout( RuntimeDataBuffer& data ) const {}

   /**
    * Creates a custom input that corresponds to this output in terms of the data type.
    *
    * @param name
    */
   virtual GBNodeInput< TNode >* createInput( const char* name ) const {
      return NULL; 
   }

   /**
    * Returns the type of data this output propagates.
    */
   virtual const ReflectionType* getDataType() const { return NULL; }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Input socket of a node.
 */
template< typename TNode >
class GBNodeInput : public GBNodeSocket
{
   DECLARE_ALLOCATOR( GBNodeInput, AM_ALIGNED_16 );

protected:
   GBNodeOutput< TNode >*             m_connectedOutput;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   GBNodeInput( const std::string& name = "" );

   /**
    * Copy constructor.
    */
   GBNodeInput( const GBNodeInput& rhs );

   virtual ~GBNodeInput();

   /**
    * Connects the socket to an output socket.
    * 
    * @param output
    *
    * @return     'true', if the connection was established successfully, 'false' otherwise
    */
   bool connect( GBNodeOutput< TNode >& output );

   /**
    * Disconnects the socket.
    */
   void disconnect();

   /**
    * Tests if the input is connected to an output.
    */
   inline bool isConnected() const { return m_connectedOutput != NULL; }

   /**
    * Returns the output socket this socket is connected to, or NULL if the socket
    * isn't connected to anything.
    */
   inline GBNodeOutput< TNode >* getOutput() const { return m_connectedOutput; }

   /**
   * Creates a custom output that corresponds to this input in terms of the data type.
   *
   * @param name
   */
   virtual GBNodeOutput< TNode >* createOutput( const char* name ) const {
      return NULL;
   }

   /**
    * Returns the type of data this output propagates.
    */
   virtual const ReflectionType* getDataType() const { return NULL; }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Data type sensitive output - this type of output is responsible for the data exchange.
 */
template< typename TNode, typename TData >
class TGBNodeOutput : public GBNodeOutput< TNode >
{
   DECLARE_ALLOCATOR( TGBNodeOutput, AM_ALIGNED_16 );

private:
   TRuntimeVar< TData >          m_value;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   TGBNodeOutput( const std::string& name = "" );

   /**
    * Copy constructor.
    */
   TGBNodeOutput( const TGBNodeOutput& rhs );
   virtual ~TGBNodeOutput();

   /**
    * Sets the new value in the socket.
    *
    * @param val
    */
   void setValue( RuntimeDataBuffer& data, const TData& val ) const;

   /**
    * Returns the socket's value.
    */
   virtual const TData& getValue( RuntimeDataBuffer& data ) const;

   // -------------------------------------------------------------------------
   // GBNodeOutput implementation
   // -------------------------------------------------------------------------
   void createLayout( RuntimeDataBuffer& data ) const;
   void destroyLayout( RuntimeDataBuffer& data ) const;
   const ReflectionType* getDataType() const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Data type sensitive input - this type of output is responsible for the data exchange.
 */
template< typename TNode, typename TData >
class TGBNodeInput : public GBNodeInput< TNode >
{
   DECLARE_ALLOCATOR( TGBNodeInput, AM_ALIGNED_16 );

public:
   /**
    * Constructor.
    *
    * @param name
    */
   TGBNodeInput( const std::string& name = "" );

   /**
    * Copy constructor.
    */
   TGBNodeInput( const TGBNodeInput& rhs );
   virtual ~TGBNodeInput();

   /**
    * Returns the socket's value.
    */
   const TData& getValue( RuntimeDataBuffer& data ) const;

   // -------------------------------------------------------------------------
   // GBNodeInput implementation
   // -------------------------------------------------------------------------
   const ReflectionType* getDataType() const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Data type sensitive output - this type of output is responsible for the objects exchange.
 */
template< typename TNode, typename TData >
class TGBNodePtrOutput : public GBNodeOutput< TNode >
{
   DECLARE_ALLOCATOR( TGBNodePtrOutput, AM_ALIGNED_16 );

private:
   TRuntimeVar< TData* >         m_value;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   TGBNodePtrOutput( const std::string& name = "" );

   /**
    * Copy constructor.
    */
   TGBNodePtrOutput( const TGBNodePtrOutput& rhs );
   virtual ~TGBNodePtrOutput();

   /**
    * Sets the new value in the socket.
    *
    * @param val
    */
   void setValue( RuntimeDataBuffer& data, TData* val ) const;

   /**
    * Returns the socket's value.
    */
   virtual TData* getValue( RuntimeDataBuffer& data ) const;

   // -------------------------------------------------------------------------
   // GBNodeOutput implementation
   // -------------------------------------------------------------------------
   void createLayout( RuntimeDataBuffer& data ) const;
   void destroyLayout( RuntimeDataBuffer& data ) const;
   const ReflectionType* getDataType() const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Data type sensitive input - this type of output is responsible for the objects exchange.
 */
template< typename TNode, typename TData >
class TGBNodePtrInput : public GBNodeInput< TNode >
{
   DECLARE_ALLOCATOR( TGBNodePtrInput, AM_ALIGNED_16 );

public:
   /**
    * Constructor.
    *
    * @param name
    */
   TGBNodePtrInput( const std::string& name = "" );

   /**
    * Copy constructor.
    */
   TGBNodePtrInput( const TGBNodePtrInput& rhs );
   virtual ~TGBNodePtrInput();

   /**
    * Returns the socket's value.
    */
   TData* getValue( RuntimeDataBuffer& data ) const;

   // -------------------------------------------------------------------------
   // GBNodeInput implementation
   // -------------------------------------------------------------------------
   const ReflectionType* getDataType() const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core/GraphBuilderSockets.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BUILDER_SOCKETS_H
