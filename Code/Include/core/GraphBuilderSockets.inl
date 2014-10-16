#ifndef _GRAPH_BUILDER_SOCKETS_H
#error "This file can only be included from GraphBuilderSockets.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
GBNodeOutput< TNode >::GBNodeOutput( const std::string& name )
   : GBNodeSocket( name )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
GBNodeOutput< TNode >::GBNodeOutput( const GBNodeOutput& rhs )
   : GBNodeSocket( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
GBNodeOutput< TNode >::~GBNodeOutput()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void GBNodeOutput< TNode >::collectConnectedNodes( List< TNode* >& outNodes )
{
   uint count = m_connectedNodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      TNode* node = m_connectedNodes[i];
      if ( node )
      {
         outNodes.pushBack( node );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void GBNodeOutput< TNode >::connect( TNode& node )
{
   // we don't want duplicates - add the node only if it's not already on the list
   std::vector< TNode* >::iterator it = std::find( m_connectedNodes.begin(), m_connectedNodes.end(), &node );
   if ( it == m_connectedNodes.end() )
   {
      m_connectedNodes.push_back( &node );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void GBNodeOutput< TNode >::disconnect( TNode& node )
{
   std::vector< TNode* >::iterator it = std::find( m_connectedNodes.begin(), m_connectedNodes.end(), &node );
   if ( it != m_connectedNodes.end() )
   {
      m_connectedNodes.erase( it );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
GBNodeInput< TNode >::GBNodeInput( const std::string& name )
   : GBNodeSocket( name )
   , m_connectedOutput( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
GBNodeInput< TNode >::GBNodeInput( const GBNodeInput& rhs )
   : GBNodeSocket( rhs )
   , m_connectedOutput( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
GBNodeInput< TNode >::~GBNodeInput()
{
   m_connectedOutput = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
bool GBNodeInput< TNode >::connect( GBNodeOutput< TNode >& output )
{
   if ( m_connectedOutput != NULL )
   {
      return false;
   }
   else
   {
      m_connectedOutput = &output;
      return true;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void GBNodeInput< TNode >::disconnect()
{
   m_connectedOutput = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodeOutput< TNode, TData >::TGBNodeOutput( const std::string& name )
   : GBNodeOutput< TNode >( name )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodeOutput< TNode, TData >::TGBNodeOutput( const TGBNodeOutput& rhs )
   : GBNodeOutput< TNode >( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodeOutput< TNode, TData >::~TGBNodeOutput()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
const ReflectionType* TGBNodeOutput< TNode, TData >::getDataType() const
{
   ReflectionTypesRegistry& registry = TSingleton< ReflectionTypesRegistry >::getInstance();
   return registry.find< TData >();
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
void TGBNodeOutput< TNode, TData >::createLayout( RuntimeDataBuffer& data ) const
{
   data.registerVar( m_value );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
void TGBNodeOutput< TNode, TData >::destroyLayout( RuntimeDataBuffer& data ) const
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
void TGBNodeOutput< TNode, TData >::setValue( RuntimeDataBuffer& data, const TData& val ) const
{
   data[ m_value ] = val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
const TData& TGBNodeOutput< TNode, TData >::getValue( RuntimeDataBuffer& data ) const
{
   return data[ m_value ];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodeInput< TNode, TData >::TGBNodeInput( const std::string& name )
   : GBNodeInput< TNode >( name )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodeInput< TNode, TData >::TGBNodeInput( const TGBNodeInput& rhs )
   : GBNodeInput< TNode >( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodeInput< TNode, TData >::~TGBNodeInput()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
const TData& TGBNodeInput< TNode, TData >::getValue( RuntimeDataBuffer& data ) const
{
   if ( !m_connectedOutput )
   {
      static TData nullData;
      return nullData;
   }

   const TGBNodeOutput< TNode, TData >* typedOutput = dynamic_cast< const TGBNodeOutput< TNode, TData >* >( m_connectedOutput );
   if ( typedOutput != NULL )
   {
      return typedOutput->getValue( data );
   }
   else
   {
      char tmp[256];
      sprintf_s( tmp, "Output '%s' input '%s' is connected to a valid type of output", m_connectedOutput->getName().c_str(), getName().c_str() );
      ASSERT_MSG( false, tmp );

      static TData nullData;
      return nullData;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
const ReflectionType* TGBNodeInput< TNode, TData >::getDataType() const
{
   ReflectionTypesRegistry& registry = TSingleton< ReflectionTypesRegistry >::getInstance();
   return registry.find< TData >();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodePtrOutput< TNode, TData >::TGBNodePtrOutput( const std::string& name )
   : GBNodeOutput< TNode >( name )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodePtrOutput< TNode, TData >::TGBNodePtrOutput( const TGBNodePtrOutput& rhs )
   : GBNodeOutput< TNode >( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodePtrOutput< TNode, TData >::~TGBNodePtrOutput()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
const ReflectionType* TGBNodePtrOutput< TNode, TData >::getDataType() const
{
   ReflectionTypesRegistry& registry = TSingleton< ReflectionTypesRegistry >::getInstance();
   return registry.find< TData >();
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
void TGBNodePtrOutput< TNode, TData >::createLayout( RuntimeDataBuffer& data ) const
{
   data.registerVar( m_value, (TData*)NULL );
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
void TGBNodePtrOutput< TNode, TData >::destroyLayout( RuntimeDataBuffer& data ) const
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
void TGBNodePtrOutput< TNode, TData >::setValue( RuntimeDataBuffer& data, TData* val ) const
{
   data[ m_value ] = val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TData* TGBNodePtrOutput< TNode, TData >::getValue( RuntimeDataBuffer& data ) const
{
   return data[ m_value ];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodePtrInput< TNode, TData >::TGBNodePtrInput( const std::string& name )
   : GBNodeInput< TNode >( name )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodePtrInput< TNode, TData >::TGBNodePtrInput( const TGBNodePtrInput& rhs )
   : GBNodeInput< TNode >( rhs )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TGBNodePtrInput< TNode, TData >::~TGBNodePtrInput()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
TData* TGBNodePtrInput< TNode, TData >::getValue( RuntimeDataBuffer& data ) const
{
   if ( !m_connectedOutput )
   {
      return NULL;
   }

   const TGBNodePtrOutput< TNode, TData >* typedOutput = static_cast< const TGBNodePtrOutput< TNode, TData >* >( m_connectedOutput );
   if ( typedOutput != NULL )
   {
      return typedOutput->getValue( data );
   }
   else
   {
      char tmp[256];
      sprintf_s( tmp, "Output '%s' input '%s' is connected to a valid type of output", m_connectedOutput->getName().c_str(), getName().c_str() );
      ASSERT_MSG( false, tmp );

      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode, typename TData >
const ReflectionType* TGBNodePtrInput< TNode, TData >::getDataType() const
{
   ReflectionTypesRegistry& registry = TSingleton< ReflectionTypesRegistry >::getInstance();
   return registry.find< TData >();
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BUILDER_SOCKETS_H
