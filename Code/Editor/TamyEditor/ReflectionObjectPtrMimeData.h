/// @file   TamyEditor\ReflectionObjectPtrMimeData.h
/// @brief  ReflectionObject pointer drag&drop related data
#ifndef _REFLECTION_OBJECT_PTR_MIME_DATA_H
#define _REFLECTION_OBJECT_PTR_MIME_DATA_H

#include "MimeDataEncoder.h"


///////////////////////////////////////////////////////////////////////////////

class ReflectionObject;
class SerializableReflectionType;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class manages the data used during a  drag&drop operation
 * of a pointer to ReflectionObject.
 */
class ReflectionObjectPtrMimeData : public MimeDataEncoder
{
private:
   const SerializableReflectionType*      m_acceptedBaseType;
   ReflectionObject*&                     m_objPtr;

public:
   /**
    * Constructor.
    *
    * @param acceptedBaseType    type of ReflectionObject that's accepted by this encoder instance
    * @param objPtr              pointer to an object we want to drag
    */
   ReflectionObjectPtrMimeData( const SerializableReflectionType& acceptedBaseType, ReflectionObject*& objPtr );

   /**
    * Constructor.
    *
    * @param AcceptedBaseType    type of ReflectionObject that's accepted by this encoder instance
    * @param objPtr              pointer to an object we want to drag
    */
   template< typename AcceptedBaseType >
   ReflectionObjectPtrMimeData( ReflectionObject*& objPtr );

   // -------------------------------------------------------------------------
   // Static API
   // -------------------------------------------------------------------------
   static void save( ReflectionObject* objPtr, QMimeData& outData );
   static void load( ReflectionObject*& outObjPtr, const QMimeData& data );

   // -------------------------------------------------------------------------
   // MimeDataEncoder implementation
   // -------------------------------------------------------------------------
   void load( const QMimeData& data );
   bool canAccept( const QMimeData& data ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "ReflectionObjectPtrMimeData.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_OBJECT_PTR_MIME_DATA_H
