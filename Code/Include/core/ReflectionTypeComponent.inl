#ifndef _REFLECTION_TYPE_COMPONENT
#error "This file can only be included from ReflectionTypeComponent.h"
#else

#include "core/StringUtils.h"
#include "core/ReflectionProperty.h"
#include "core/ReflectionDependenciesCallback.h"
#include "core/PatchRecord.h"


///////////////////////////////////////////////////////////////////////////////

uint ReflectionTypeComponent::generateId( const std::string& memberName )
{
   return StringUtils::calculateHash( memberName );
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionTypeComponent::savePtr( const void* dataPtr, const ReflectionDependencyMapperCallback& dependenciesMapper, uint& outMappedPtr ) const
{
   bool isPtrNull = (dataPtr == NULL);

   if ( dataPtr != NULL )
   {
      outMappedPtr = dependenciesMapper.findDependency( reinterpret_cast< const ReflectionObject* >( dataPtr ) );
   }
   else
   {
      outMappedPtr = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< T >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const T* dataPtr = reinterpret_cast< const T* >( memberPtr );

   savedObject.addValue( m_id ).set< T >( *dataPtr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   if ( object )
   {
      char* memberPtr = (char*)object + m_dataOffset;

      const SerializedValue* value = savedObject.getValue( m_id );
      if ( value )
      {
         value->initialize< T >( *reinterpret_cast< T* >( memberPtr ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   // there are no dependencies among simple types, just pointers
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   // there are no dependencies among simple types, just pointers
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< T >::instantiateProperty( void* object ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   T* dataPtr = reinterpret_cast< T* >( memberPtr );

   return new TReflectionProperty< T >( (ReflectionObject*)object, dataPtr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_Value );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< T* >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T* >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   T* const * dataPtr = reinterpret_cast< T* const * >( memberPtr );

   uint mappedPtrVal;
   savePtr( *dataPtr, dependenciesMapper, mappedPtrVal );
   savedObject.addPointer( m_id ).set( ( const void* )mappedPtrVal );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T* >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   if ( object )
   {
      char* memberPtr = (char*)object + m_dataOffset;
      T** dataPtr = reinterpret_cast< T** >( memberPtr );

      const SerializedPointer* pointer = savedObject.getPointer( m_id );
      if ( pointer )
      {
         pointer->initialize( *(void**)dataPtr );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T* >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   T* const * dataPtr = reinterpret_cast< T* const * >( memberPtr );

   // add the stored object as a dependency
   const T* storedObject = static_cast< const T* >( *dataPtr );
   dependenciesCollector.addDependency( storedObject );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T* >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   T** dataPtr = reinterpret_cast< T** >( memberPtr );

   // the pointer contains a dependency index - now we just have to find a corresponding object
   bool wasRestored = false;
   T* restoredObject = reinterpret_cast< T* >( dependenciesLinker.findDependency( (uint)(*dataPtr), wasRestored ) );
   *dataPtr = restoredObject;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< T* >::instantiateProperty( void* object ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   T** dataPtr = reinterpret_cast< T** >( memberPtr );

   return new TReflectionProperty< T* >( (ReflectionObject*)object, dataPtr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< T* >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_Pointer );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< TRefPtr< T > >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TRefPtr< T > >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   TRefPtr< T > const * dataPtr = reinterpret_cast< TRefPtr< T > const * >( memberPtr );

   uint mappedPtrVal;
   savePtr( dataPtr->get(), dependenciesMapper, mappedPtrVal );
   savedObject.addPointer( m_id ).set( (const void*)mappedPtrVal );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TRefPtr< T > >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   if ( object )
   {
      void* restoredPtr = NULL;
      const SerializedPointer* pointer = savedObject.getPointer( m_id );
      if ( pointer )
      {
         pointer->initialize( restoredPtr );
      }

      char* memberPtr = (char*)object + m_dataOffset;

      TRefPtr< T >* dataPtr = new ( memberPtr ) TRefPtr< T >();
      dataPtr->setDuringSerialization( (T*)restoredPtr );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TRefPtr< T > >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   TRefPtr< T > const * dataPtr = reinterpret_cast< TRefPtr< T > const * >( memberPtr );

   // add the stored object as a dependency
   const T* storedObject = static_cast< const T* >( dataPtr->get() ) ;
   dependenciesCollector.addDependency( storedObject );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TRefPtr< T > >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   TRefPtr< T >* dataPtr = reinterpret_cast< TRefPtr< T >* >( memberPtr );

   // the pointer contains a dependency index - now we just have to find a corresponding object
   bool wasRestored = false;
   T* restoredObject = reinterpret_cast< T* >( dependenciesLinker.findDependency( (uint)( dataPtr->get() ), wasRestored ) );
   if ( wasRestored )
   {
      restoredObject->addReference();
   }
   dataPtr->setDuringSerialization( restoredObject );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< TRefPtr< T > >::instantiateProperty( void* object ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   TRefPtr< T >* dataPtr = reinterpret_cast< TRefPtr< T >* >( memberPtr );

   return new TReflectionProperty< TRefPtr< T > >( (ReflectionObject*)object, dataPtr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TRefPtr< T > >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_Pointer );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< std::vector< T > >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T > >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const std::vector< T >* dataPtr = reinterpret_cast< const std::vector< T >* >( memberPtr );

   // serialize the number of entries
   SerializedArrayOfValues& arr = savedObject.addArrayOfValues( m_id );
   uint count = dataPtr->size();
   arr.setSize( count );

   for ( uint i = 0; i < count; ++i )
   {
      arr.set< T >( i, (*dataPtr)[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T > >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   char* memberPtr = (char*)object + m_dataOffset;
   std::vector< T >* dataPtr = reinterpret_cast< std::vector< T >* >( memberPtr );

   // deserialize the number of entries
   const SerializedArrayOfValues* arr = savedObject.getArrayOfValues( m_id );
   if ( !arr )
   {
      return;
   }

   // make place in the array
   uint count = arr->size();
   dataPtr->resize( count );

   // deserialize the entries
   for ( uint i = 0; i < count; ++i )
   {
      arr->initialize< T >( i, (*dataPtr)[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T > >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   // there are no dependencies among simple types, just pointers
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T > >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   // there are no dependencies among simple types, just pointers
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< std::vector< T > >::instantiateProperty( void* object ) const
{
   // at the moment these properties can't be edited
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T > >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_ArrayOfValues );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< std::vector< T* > >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T* > >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const std::vector< T* >* dataPtr = reinterpret_cast< const std::vector< T* >* >( memberPtr );

   // serialize the number of entries
   SerializedArrayOfPointers& arr = savedObject.addArrayOfPointers( m_id );
   uint count = dataPtr->size();
   arr.setSize( count );

   uint mappedPtrVal;
   for ( uint i = 0; i < count; ++i )
   {
      savePtr( (*dataPtr)[i], dependenciesMapper, mappedPtrVal );
      arr.set( i, (const void*)mappedPtrVal );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T* > >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   char* memberPtr = (char*)object + m_dataOffset;
   std::vector< T* >* dataPtr = reinterpret_cast< std::vector< T* >* >( memberPtr );

   // deserialize the number of entries
   const SerializedArrayOfPointers* arr = savedObject.getArrayOfPointers( m_id );
   if ( !arr )
   {
      return;
   }

   // make place in the array
   uint count = arr->size();
   dataPtr->resize( count );

   // deserialize the entries
   for ( uint i = 0; i < count; ++i )
   {
      void* restoredPtr = NULL;
      arr->initialize( i, restoredPtr );
      (*dataPtr)[i] = (T*)restoredPtr;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T* > >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const std::vector< T* >* dataPtr = reinterpret_cast< const std::vector< T* >* >( memberPtr );

   uint count = dataPtr->size();
   for ( uint i = 0; i < count; ++i )
   {
      // add the stored object as a dependency
      const T* storedObject = reinterpret_cast< const T* >( (*dataPtr)[i] ) ;
      dependenciesCollector.addDependency( storedObject );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T* > >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   std::vector< T* >* dataPtr = reinterpret_cast< std::vector< T* >* >( memberPtr );

   uint count = dataPtr->size();
   for ( uint i = 0; i < count; ++i )
   {
      // the pointers contain indices to the actual dependencies - now we just have to find corresponding objects
      bool wasRestored = false;
      T* restoredObject = reinterpret_cast< T* >( dependenciesLinker.findDependency( (uint)(*dataPtr)[i], wasRestored ) );
      (*dataPtr)[i] = restoredObject;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< std::vector< T* > >::instantiateProperty( void* object ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   std::vector< T* >* dataPtr = reinterpret_cast< std::vector< T* >* >( memberPtr );

   return new TReflectionProperty< std::vector< T* > >( (ReflectionObject*)object, dataPtr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< T* > >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_ArrayOfPointers );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< std::vector< TRefPtr< T > > >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< TRefPtr< T > > >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const std::vector< TRefPtr< T > >* dataPtr = reinterpret_cast< const std::vector< TRefPtr< T > >* >( memberPtr );

   // serialize the number of entries
   SerializedArrayOfPointers& arr = savedObject.addArrayOfPointers( m_id );
   uint count = dataPtr->size();
   arr.setSize( count );

   uint mappedPtrVal;
   for ( uint i = 0; i < count; ++i )
   {
      savePtr( (*dataPtr)[i].get(), dependenciesMapper, mappedPtrVal );
      arr.set( i, (const void*)mappedPtrVal );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< TRefPtr< T > > >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   char* memberPtr = (char*)object + m_dataOffset;
   std::vector< TRefPtr< T > >* dataPtr = reinterpret_cast< std::vector< TRefPtr< T > >* >( memberPtr );

   // deserialize the number of entries
   const SerializedArrayOfPointers* arr = savedObject.getArrayOfPointers( m_id );
   if ( !arr )
   {
      return;
   }

   // make place in the array
   uint count = arr->size();
   dataPtr->resize( count );

   // deserialize the entries
   for ( uint i = 0; i < count; ++i )
   {
      void* restoredPtr = NULL;
      arr->initialize( i, restoredPtr );

      (*dataPtr)[i].setDuringSerialization( (T*)restoredPtr );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< TRefPtr< T > > >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const std::vector< TRefPtr< T > >* dataPtr = reinterpret_cast< const std::vector< TRefPtr< T > >* >( memberPtr );

   uint count = dataPtr->size();
   for ( uint i = 0; i < count; ++i )
   {
      // add the stored object as a dependency
      const T* storedObject = reinterpret_cast< const T* >( (*dataPtr)[i].get() ) ;
      dependenciesCollector.addDependency( storedObject );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< TRefPtr< T > > >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   std::vector< TRefPtr< T > >* dataPtr = reinterpret_cast< std::vector< TRefPtr< T > >* >( memberPtr );

   uint count = dataPtr->size();
   for ( uint i = 0; i < count; ++i )
   {
      // the pointers contain indices to the actual dependencies - now we just have to find corresponding objects
      bool wasRestored = false;
      T* restoredObject = reinterpret_cast< T* >( dependenciesLinker.findDependency( (uint)(*dataPtr)[i].get(), wasRestored ) );
      if ( wasRestored )
      {
         restoredObject->addReference();
      }
      (*dataPtr)[i].setDuringSerialization( restoredObject );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< std::vector< TRefPtr< T > > >::instantiateProperty( void* object ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   std::vector< TRefPtr< T > >* dataPtr = reinterpret_cast< std::vector< TRefPtr< T > >* >( memberPtr );

   return new TReflectionProperty< std::vector< TRefPtr< T > > >( (ReflectionObject*)object, dataPtr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< std::vector< TRefPtr< T > > >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_ArrayOfPointers );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< Array< T > >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T > >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const Array< T >* dataPtr = reinterpret_cast< const Array< T >* >( memberPtr );

   // serialize the number of entries
   SerializedArrayOfValues& arr = savedObject.addArrayOfValues( m_id );
   uint count = dataPtr->size();
   arr.setSize( count );

   for ( uint i = 0; i < count; ++i )
   {
      arr.set< T >( i, (*dataPtr)[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T > >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   char* memberPtr = (char*)object + m_dataOffset;
   Array< T >* dataPtr = reinterpret_cast< Array< T >* >( memberPtr );

   // deserialize the number of entries
   const SerializedArrayOfValues* arr = savedObject.getArrayOfValues( m_id );
   if ( !arr )
   {
      return;
   }

   // make place in the array
   uint count = arr->size();
   dataPtr->resizeWithoutInitializing( count );

   // deserialize the entries
   for ( uint i = 0; i < count; ++i )
   {
      arr->initialize< T >( i, (*dataPtr)[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T > >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   // there are no dependencies among simple types, just pointers
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T > >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   // there are no dependencies among simple types, just pointers
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< Array< T > >::instantiateProperty( void* object ) const
{
   // at the moment these properties can't be edited
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T > >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_ArrayOfValues );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< Array< T* > >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T* > >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const Array< T* >* dataPtr = reinterpret_cast< const Array< T* >* >( memberPtr );

   // serialize the number of entries
   SerializedArrayOfPointers& arr = savedObject.addArrayOfPointers( m_id );
   uint count = dataPtr->size();
   arr.setSize( count );

   uint mappedPtrVal;

   for ( uint i = 0; i < count; ++i )
   {
      savePtr( (*dataPtr)[i], dependenciesMapper, mappedPtrVal );
      arr.set( i, (const void*)mappedPtrVal );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T* > >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   char* memberPtr = (char*)object + m_dataOffset;
   Array< T* >* dataPtr = reinterpret_cast< Array< T* >* >( memberPtr );

   // deserialize the number of entries
   const SerializedArrayOfPointers* arr = savedObject.getArrayOfPointers( m_id );
   if ( !arr )
   {
      return;
   }

   // make place in the array
   uint count = arr->size();
   dataPtr->resize( count );

   // deserialize the entries
   for ( uint i = 0; i < count; ++i )
   {
      void* restoredPtr = NULL;
      arr->initialize( i, restoredPtr );
      (*dataPtr)[i] = (T*)restoredPtr;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T* > >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const Array< T* >* dataPtr = reinterpret_cast< const Array< T* >* >( memberPtr );

   uint count = dataPtr->size();
   for ( uint i = 0; i < count; ++i )
   {
      // add the stored object as a dependency
      const T* storedObject = reinterpret_cast< const T* >( (*dataPtr)[i] ) ;
      dependenciesCollector.addDependency( storedObject );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T* > >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   Array< T* >* dataPtr = reinterpret_cast< Array< T* >* >( memberPtr );

   uint count = dataPtr->size();
   for ( uint i = 0; i < count; ++i )
   {
      // the pointers contain indices to the actual dependencies - now we just have to find corresponding objects
      bool wasRestored = false;
      T* restoredObject = reinterpret_cast< T* >( dependenciesLinker.findDependency( (uint)(*dataPtr)[i], wasRestored ) );
      (*dataPtr)[i] = restoredObject;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< Array< T* > >::instantiateProperty( void* object ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   Array< T* >* dataPtr = reinterpret_cast< Array< T* >* >( memberPtr );

   return new TReflectionProperty< Array< T* > >( (ReflectionObject*)object, dataPtr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< T* > >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_ArrayOfPointers );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< Array< TRefPtr< T > > >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< TRefPtr< T > > >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const Array< TRefPtr< T > >* dataPtr = reinterpret_cast< const Array< TRefPtr< T > >* >( memberPtr );

   // serialize the number of entries
   SerializedArrayOfPointers& arr = savedObject.addArrayOfPointers( m_id );
   uint count = dataPtr->size();
   arr.setSize( count );

   uint mappedPtrVal;

   for ( uint i = 0; i < count; ++i )
   {
      savePtr( (*dataPtr)[i], dependenciesMapper, mappedPtrVal );
      arr.set( i, (const void*)mappedPtrVal );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< TRefPtr< T > > >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   char* memberPtr = (char*)object + m_dataOffset;
   Array< TRefPtr< T > >* dataPtr = reinterpret_cast< Array< TRefPtr< T > >* >( memberPtr );

   // deserialize the number of entries
   const SerializedArrayOfPointers* arr = savedObject.getArrayOfPointers( m_id );
   if ( !arr )
   {
      return;
   }

   // make place in the array
   uint count = arr->size();
   dataPtr->resize( count );

   // deserialize the entries
   for ( uint i = 0; i < count; ++i )
   {
      void* restoredPtr = NULL;
      arr->initialize( i, restoredPtr );

      (*dataPtr)[i].setDuringSerialization( (T*)restoredPtr );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< TRefPtr< T > > >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const Array< TRefPtr< T > >* dataPtr = reinterpret_cast< const Array< TRefPtr< T > >* >( memberPtr );

   uint count = dataPtr->size();
   for ( uint i = 0; i < count; ++i )
   {
      // add the stored object as a dependency
      const T* storedObject = reinterpret_cast< const T* >( (*dataPtr)[i].get() ) ;
      dependenciesCollector.addDependency( storedObject );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< TRefPtr< T > > >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   Array< TRefPtr< T > >* dataPtr = reinterpret_cast< Array< TRefPtr< T > >* >( memberPtr );

   uint count = dataPtr->size();
   for ( uint i = 0; i < count; ++i )
   {
      // the pointers contain indices to the actual dependencies - now we just have to find corresponding objects
      bool wasRestored = false;
      T* restoredObject = reinterpret_cast< T* >( dependenciesLinker.findDependency( (uint)(*dataPtr)[i].get(), wasRestored ) );
      if ( wasRestored )
      {
         restoredObject->addReference();
      }
      (*dataPtr)[i].setDuringSerialization( restoredObject );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< Array< TRefPtr< T > > >::instantiateProperty( void* object ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   Array< TRefPtr< T > >* dataPtr = reinterpret_cast< Array< TRefPtr< T > >* >( memberPtr );

   return new TReflectionProperty< Array< TRefPtr< T > > >( (ReflectionObject*)object, dataPtr );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< Array< TRefPtr< T > > >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_ArrayOfPointers );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TMemberField< TShared< T > >::TMemberField( const std::string& memberName, int offset ) 
   : ReflectionTypeComponent( memberName, offset )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TShared< T > >::save( const void* object, const ReflectionDependencyMapperCallback& dependenciesMapper, SerializedReflectionObject& savedObject ) const
{
   const char* memberPtr = (const char*)object + m_dataOffset;
   const TShared< T >* dataPtr = reinterpret_cast< const TShared< T >* >( memberPtr );

   savedObject.addValue( m_id ).set< T >( dataPtr->get() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TShared< T > >::load( void* object, const SerializedReflectionObject& savedObject ) const 
{
   if ( object )
   {
      char* memberPtr = (char*)object + m_dataOffset;

      const SerializedValue* value = savedObject.getValue( m_id );
      if ( value )
      {
         TShared< T >* sharedVal = reinterpret_cast< TShared< T >* >( memberPtr );
         value->initialize< T >( sharedVal->get() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TShared< T > >::mapDependencies( const void* object, ReflectionDependencyMapperCallback& dependenciesCollector ) const
{
   // there are no dependencies among simple types, just pointers
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TShared< T > >::restoreDependencies( void* object, const ReflectionDependencyLinkerCallback& dependenciesLinker ) const
{
   // there are no dependencies among simple types, just pointers
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
ReflectionProperty* TMemberField< TShared< T > >::instantiateProperty( void* object ) const
{
   char* memberPtr = (char*)object + m_dataOffset;
   TShared< T >* dataPtr = reinterpret_cast< TShared< T >* >( memberPtr );

   return new TReflectionProperty< T >( (ReflectionObject*)object, dataPtr->getPtr() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TMemberField< TShared< T > >::addToPatchRecord( PatchRecord& record ) const
{
   record.addField( m_memberName, SFT_Value );
}

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_TYPE_COMPONENT
