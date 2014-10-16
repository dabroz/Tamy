#ifndef REFLECTION_PROPERTY_H
#error "This file can only be included from ReflectionProperty.h"
#else

#include "core\ReflectionType.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

template< typename RhsCollectionType, typename LhsCollectionType >
static bool compareCollection( const RhsCollectionType& rhs, const LhsCollectionType& lhs )
{
   uint oldVecSize = rhs.size();
   uint newVecSize = lhs.size();
   bool hasChanged = false;

   if ( oldVecSize != newVecSize )
   {
      hasChanged = true;
   }
   else
   {
      for ( uint i = 0; i < oldVecSize; ++i )
      {
         if ( rhs[i] != lhs[i] )
         {
            hasChanged = true;
            break;
         }
      }
   }

   return !hasChanged;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ReflectionProperty::setParams( uint id, const std::string& name, const std::string& label, byte traits )
{
   m_id = id;
   m_name = name;
   m_label = label;
   m_traits = traits;
}

///////////////////////////////////////////////////////////////////////////////

uint ReflectionProperty::getId() const
{
   return m_id;
}

///////////////////////////////////////////////////////////////////////////////

const std::string& ReflectionProperty::getName() const
{
   return m_name;
}

///////////////////////////////////////////////////////////////////////////////

const std::string& ReflectionProperty::getLabel() const
{
   return ReflectionProperty::m_label;
}

///////////////////////////////////////////////////////////////////////////////

bool ReflectionProperty::isReference() const
{
   return ( m_traits & ReflectionTypeComponent::RTC_Reference ) == ReflectionTypeComponent::RTC_Reference;
}

///////////////////////////////////////////////////////////////////////////////

bool ReflectionProperty::canBeEdited() const
{
   return ( m_traits & ReflectionTypeComponent::RTC_Editable ) == ReflectionTypeComponent::RTC_Editable;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TReflectionProperty< T >::TReflectionProperty( ReflectionObject* hostObject, T* val ) 
   : ReflectionProperty( hostObject )
   , m_val( val )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TReflectionProperty< T >::set( void* val )
{
   T* newPtrVal = reinterpret_cast< T* >( val );
   if ( !isApproximatelyEqual< T >( *m_val, *newPtrVal ) )
   {
      notifyBeforeChange();
      *m_val = *newPtrVal;
      notifyAfterChange();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TReflectionProperty< T >::edit()
{
   return m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType* TReflectionProperty< T >::getVirtualClass() const
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   return type;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< T >::getPropertyClass() const
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< T >::getRTTIClass()
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TReflectionProperty< T* >::TReflectionProperty( ReflectionObject* hostObject, T** val ) 
   : ReflectionProperty( hostObject )
   , m_val(val)
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TReflectionProperty< T* >::set( void* val )
{
   T** newPtrVal = reinterpret_cast< T** >( val ) ;
   if ( *m_val != *newPtrVal )
   {
      notifyBeforeChange();
      *m_val = *newPtrVal;
      notifyAfterChange();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TReflectionProperty< T* >::edit()
{
   return m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType* TReflectionProperty< T* >::getVirtualClass() const
{
   if ( *m_val )
   {
      return &(*m_val)->getVirtualRTTI();
   }
   else
   {
      const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
      return type;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< T* >::getPropertyClass() const
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< T* >::getRTTIClass()
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TReflectionProperty< TRefPtr< T > >::TReflectionProperty( ReflectionObject* hostObject, TRefPtr< T >* val ) 
   : ReflectionProperty( hostObject )
   , m_val(val)
   , m_tmpVal( m_val->get() )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TReflectionProperty< TRefPtr< T > >::set( void* val )
{
   T** newPtrVal = reinterpret_cast< T** >( val ) ;
   if ( *m_val != *newPtrVal )
   {
      notifyBeforeChange();
      m_tmpVal = *newPtrVal;
      *m_val = m_tmpVal;
      notifyAfterChange();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TReflectionProperty< TRefPtr< T > >::edit()
{
   return &m_tmpVal;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType* TReflectionProperty< TRefPtr< T > >::getVirtualClass() const
{
   if ( m_val->isNotNull() )
   {
      return &(*m_val)->getVirtualRTTI();
   }
   else
   {
      const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();

      return type;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< TRefPtr< T > >::getPropertyClass() const
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< TRefPtr< T > >::getRTTIClass()
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TReflectionProperty< std::vector< T* > >::TReflectionProperty( ReflectionObject* hostObject, std::vector< T* >* val ) 
   : ReflectionPropertyArray( hostObject )
   , m_val( val )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TReflectionProperty< std::vector< T* > >::set( void* val )
{
   // check if the collection has changed
   std::vector< T* >* newVec = reinterpret_cast< std::vector< T* >* >( val );
   bool isEqual = compareCollection( *m_val, *newVec );
   
   if ( !isEqual )
   {
      notifyBeforeChange();
      notifyAfterChange();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TReflectionProperty< std::vector< T* > >::edit()
{
   return m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int TReflectionProperty< std::vector< T* > >::size() const
{
   return m_val->size();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionObject* TReflectionProperty< std::vector< T* > >::getElement( uint idx )
{
   return ( *m_val )[idx];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType* TReflectionProperty< std::vector< T* > >::getVirtualClass() const
{
   return &getRTTIClass();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< std::vector< T* > >::getPropertyClass() const
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< std::vector< T* > >::getRTTIClass()
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TReflectionProperty< std::vector< TRefPtr< T > > >::TReflectionProperty( ReflectionObject* hostObject, std::vector< TRefPtr< T > >* val ) 
   : ReflectionPropertyArray( hostObject )
   , m_val( val )
{
   uint count = m_val->size();
   m_tmpVal.resize( count, NULL );
   for ( uint i = 0; i < count; ++i )
   {
      m_tmpVal[i] = (*m_val)[i].get();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TReflectionProperty< std::vector< TRefPtr< T > > >::set( void* val )
{
   // check if the collection has changed
   bool isEqual = compareCollection( *m_val, m_tmpVal );
   
   if ( !isEqual )
   {
      notifyBeforeChange();

      uint oldSize = m_val->size();
      uint newSize = m_tmpVal.size();
      if ( oldSize > newSize )
      {
         uint count = oldSize - newSize;
         for ( uint i = 0; i < count; ++i )
         {
            m_val->pop_back();
         }
      }
      else if ( oldSize < newSize )
      {
         uint count = newSize - oldSize;
         for ( uint i = 0; i < count; ++i )
         {
            m_val->push_back( TRefPtr< T >() );
         }
      }

      for ( uint i = 0; i < newSize; ++i )
      {
         (*m_val)[i] = m_tmpVal[i];
      }

      notifyAfterChange();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TReflectionProperty< std::vector< TRefPtr< T > > >::edit()
{
   return &m_tmpVal;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int TReflectionProperty< std::vector< TRefPtr< T > > >::size() const
{
   return m_val->size();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionObject* TReflectionProperty< std::vector< TRefPtr< T > > >::getElement( uint idx )
{
   return ( *m_val )[idx].get();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType* TReflectionProperty< std::vector< TRefPtr< T > > >::getVirtualClass() const
{
   return getRTTIClass();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< std::vector< TRefPtr< T > > >::getPropertyClass() const
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< std::vector< TRefPtr< T > > >::getRTTIClass()
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TReflectionProperty< Array< T* > >::TReflectionProperty( ReflectionObject* hostObject, Array< T* >* val ) 
   : ReflectionPropertyArray( hostObject )
   , m_val( val )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TReflectionProperty< Array< T* > >::set( void* val )
{
   // check if the collection has changed
   Array< T* >* newVec = reinterpret_cast< Array< T* >* >( val );
   bool isEqual = compareCollection( *m_val, *newVec );
   
   if ( !isEqual )
   {
      notifyBeforeChange();
      notifyAfterChange();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TReflectionProperty< Array< T* > >::edit()
{
   return m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int TReflectionProperty< Array< T* > >::size() const
{
   return m_val->size();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionObject* TReflectionProperty< Array< T* > >::getElement( uint idx )
{
   return ( *m_val )[idx];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType* TReflectionProperty< Array< T* > >::getVirtualClass() const
{
   return &getRTTIClass();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< Array< T* > >::getPropertyClass() const
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< Array< T* > >::getRTTIClass()
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TReflectionProperty< Array< TRefPtr< T > > >::TReflectionProperty( ReflectionObject* hostObject, Array< TRefPtr< T > >* val ) 
   : ReflectionPropertyArray( hostObject )
   , m_val( val )
{
   uint count = m_val->size();
   m_tmpVal.resize( count, NULL );
   for ( uint i = 0; i < count; ++i )
   {
      m_tmpVal[i] = (*m_val)[i].get();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TReflectionProperty< Array< TRefPtr< T > > >::set( void* val )
{
   // check if the collection has changed
   bool isEqual = compareCollection( *m_val, m_tmpVal );
   
   if ( !isEqual )
   {
      notifyBeforeChange();

      uint oldSize = m_val->size();
      uint newSize = m_tmpVal.size();
      if ( oldSize > newSize )
      {
         uint count = oldSize - newSize;
         for ( uint i = 0; i < count; ++i )
         {
            m_val->pop_back();
         }
      }
      else if ( oldSize < newSize )
      {
         uint count = newSize - oldSize;
         for ( uint i = 0; i < count; ++i )
         {
            m_val->push_back( TRefPtr< T >() );
         }
      }

      for ( uint i = 0; i < newSize; ++i )
      {
         (*m_val)[i] = m_tmpVal[i];
      }

      notifyAfterChange();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TReflectionProperty< Array< TRefPtr< T > > >::edit()
{
   return &m_tmpVal;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int TReflectionProperty< Array< TRefPtr< T > > >::size() const
{
   return m_val->size();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionObject* TReflectionProperty< Array< TRefPtr< T > > >::getElement( uint idx )
{
   return ( *m_val )[idx].get();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType* TReflectionProperty< Array< TRefPtr< T > > >::getVirtualClass() const
{
   return getRTTIClass();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< Array< TRefPtr< T > > >::getPropertyClass() const
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TReflectionProperty< Array< TRefPtr< T > > >::getRTTIClass()
{
   const ReflectionType* type = TSingleton< ReflectionTypesRegistry >::getInstance().find< T >();
   ASSERT_MSG( type != NULL, "This property type doesn't exist" );

   return *type;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TEditableReflectionProperty< T >::TEditableReflectionProperty( ReflectionProperty* property )
   : m_property( property )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TEditableReflectionProperty< T >::~TEditableReflectionProperty()
{
   delete m_property;
   m_property = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const std::string& TEditableReflectionProperty< T >::getLabel() const
{
   return m_property->getLabel();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
bool TEditableReflectionProperty< T >::isReference() const
{
   return m_property->isReference();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TEditableReflectionProperty< T >::set( const T& val ) 
{
   m_property->set( ( void* )( &val ) );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& TEditableReflectionProperty< T >::get() const 
{
   void* val = m_property->edit();
   ASSERT_MSG( val != NULL, "Non-pointer properties must be initialized before thay can be edited" );

   T* typedVal = reinterpret_cast< T* >( val );

   return *typedVal;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& TEditableReflectionProperty< T >::get() 
{
   void* val = m_property->edit();
   ASSERT_MSG( val != NULL, "Non-pointer properties must be initialized before thay can be edited" );

   T* typedVal = reinterpret_cast< T* >( val );

   return *typedVal;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const ReflectionType& TEditableReflectionProperty< T >::getType() const
{
   return m_property->getPropertyClass();
}

///////////////////////////////////////////////////////////////////////////////

#endif // REFLECTION_PROPERTY_H
