/// @file   TamyEditor/ResourceMimeData.h
/// @brief  resource mime data encoder/decoder
#ifndef _RESOURCE_MIME_DATA_H
#define _RESOURCE_MIME_DATA_H

#include "MimeDataEncoder.h"


///////////////////////////////////////////////////////////////////////////////

class SerializableReflectionType;
class Resource;

///////////////////////////////////////////////////////////////////////////////

class ResourceMimeData : public MimeDataEncoder
{
private:
   const SerializableReflectionType*      m_acceptedBaseType;
   Resource*&                             m_outResource;

public:
   /**
    * Constructor.
    *
    * @param acceptedBaseType    type of the accepted resource
    * @param outResource         reference to a pointer that will be set to point at the decoded resource
    */
   ResourceMimeData( const SerializableReflectionType* acceptedBaseType, Resource*& outResource );

   /**
    * Constructor.
    *
    * @param ResourceType     decoded resource type
    * @param outResource      reference to a pointer that will be set to point at the decoded resource
    */
   template< typename ResourceType >
   static ResourceMimeData* create( ResourceType*& outResource );

   // -------------------------------------------------------------------------
   // Static API
   // -------------------------------------------------------------------------
   static void save( Resource* resourcePtr, QMimeData& outData );
   static void load( Resource*& outResourcePtr, const QMimeData& data );

   template< typename T >
   static bool containsRelevantData( const QMimeData& data );

   static bool containsRelevantData( const QMimeData& data, const SerializableReflectionType& acceptedBaseType );

   // -------------------------------------------------------------------------
   // MimeDataEncoder implementation
   // -------------------------------------------------------------------------
   void load( const QMimeData& data );
   bool canAccept( const QMimeData& data ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "ResourceMimeData.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _RESOURCE_MIME_DATA_H
