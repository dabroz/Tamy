#include "ReflectionObjectPtrMimeData.h"
#include "core\ReflectionObject.h"
#include "core\ReflectionTypesRegistry.h"
#include <QtCore\QMimeData>
#include <QtCore\QDataStream>


///////////////////////////////////////////////////////////////////////////////

ReflectionObjectPtrMimeData::ReflectionObjectPtrMimeData( const SerializableReflectionType& acceptedBaseType, ReflectionObject*& objPtr )
   : m_acceptedBaseType( &acceptedBaseType )
   , m_objPtr( objPtr )
{
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectPtrMimeData::load( const QMimeData& data )
{
   load( m_objPtr, data );
}

///////////////////////////////////////////////////////////////////////////////

bool ReflectionObjectPtrMimeData::canAccept( const QMimeData& data ) const
{
   QByteArray encodedData = data.data( "TamyEditor/ReflectionObjectPtrMimeData" );
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

   bool typeMatches = matchingType->isA( *m_acceptedBaseType );
   return typeMatches;
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectPtrMimeData::save( ReflectionObject* objPtr, QMimeData& outData )
{
   if ( objPtr )
   {
      QByteArray encodedData;
      QDataStream stream( &encodedData, QIODevice::WriteOnly );

      uint typeId = objPtr->getVirtualRTTI().m_id;
      stream << typeId;
      stream << (uint)objPtr;

      outData.setData( "TamyEditor/ReflectionObjectPtrMimeData", encodedData );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionObjectPtrMimeData::load( ReflectionObject*& outObjPtr, const QMimeData& data )
{
   QByteArray encodedData = data.data( "TamyEditor/ReflectionObjectPtrMimeData" );
   QDataStream stream( &encodedData, QIODevice::ReadOnly );

   uint typeId, ptr;
   stream >> typeId;
   stream >> ptr;
   outObjPtr = (ReflectionObject*)ptr;
}

///////////////////////////////////////////////////////////////////////////////
