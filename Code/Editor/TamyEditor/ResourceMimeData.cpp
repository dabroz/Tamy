#include "ResourceMimeData.h"
#include "core\Resource.h"
#include "core\ReflectionTypesRegistry.h"
#include <QtCore\QMimeData>
#include <QtCore\QDataStream>

///////////////////////////////////////////////////////////////////////////////

ResourceMimeData::ResourceMimeData( const SerializableReflectionType* acceptedBaseType, Resource*& outResource )
   : m_acceptedBaseType( acceptedBaseType )
   , m_outResource( outResource )
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourceMimeData::save( Resource* resourcePtr, QMimeData& outData )
{
   if ( resourcePtr )
   {
      QByteArray encodedData;
      QDataStream stream( &encodedData, QIODevice::WriteOnly );

      uint typeId = resourcePtr->getVirtualRTTI().m_id;
      stream << typeId;
      stream << (uint)resourcePtr;

      outData.setData( "TamyEditor/ResourceMimeData", encodedData );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceMimeData::load( Resource*& outResourcePtr, const QMimeData& data )
{
   QByteArray encodedData = data.data( "TamyEditor/ResourceMimeData" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   uint typeId, ptr;
   stream >> typeId;
   stream >> ptr;
   outResourcePtr = (Resource*)ptr;
}

///////////////////////////////////////////////////////////////////////////////

bool ResourceMimeData::containsRelevantData( const QMimeData& data, const SerializableReflectionType& acceptedBaseType )
{
   QByteArray encodedData = data.data( "TamyEditor/ResourceMimeData" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   // check if the stream carries any data of interest to us
   if ( stream.atEnd() )
   {
      return false;
   }

   // check if the stream contains an object of a class that derives from the one set for this encoder
   uint typeId;
   stream >> typeId;

   ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance();
   SerializableReflectionType* matchingType = typesRegistry.findSerializable( typeId );
   if ( !matchingType )
   {
      // this type wasn't defined in the registry - the stream must be corrupt
      return false;
   }

   bool typeMatches = matchingType->isA( acceptedBaseType );
   return typeMatches;
}

///////////////////////////////////////////////////////////////////////////////

void ResourceMimeData::load( const QMimeData& data )
{
   load( m_outResource, data );
}

///////////////////////////////////////////////////////////////////////////////

bool ResourceMimeData::canAccept( const QMimeData& data ) const
{
   return containsRelevantData( data, *m_acceptedBaseType );
}

///////////////////////////////////////////////////////////////////////////////
