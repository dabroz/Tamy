/// @file   core/ReflectionType.h
/// @brief  reflection type definition
#ifndef _REFLECTION_TYPE_H
#define _REFLECTION_TYPE_H

#include <vector>
#include <string>
#include <map>
#include <list>
#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class ReflectionSaver;
class ReflectionLoader;
class ReflectionTypeComponent;
class ReflectionProperty;
class OutStream;
class InStream;
class TSContinuousMemoryPool;
class SerializedReflectionObject;

///////////////////////////////////////////////////////////////////////////////

/**
 * Definition of a type that can be recognized at runtime and can serialize
 * objects defined using it.
 */
class ReflectionType
{
   DECLARE_ALLOCATOR( ReflectionType, AM_DEFAULT );

public:
   // type name and its id version
   std::string                                        m_name;
   uint                                               m_id;
   int                                                m_versionNo;

public:
   /**
    * Constructor.
    *
    * @param name             type name
    */
   ReflectionType( const std::string& name );
   virtual ~ReflectionType() {}

   /**
    * Sets a new type name.
    */
   void setTypeName( const std::string& name );

   /**
    * A utility method for generating a unique ID based on the specified type name.
    *
    * @param typeName
    */
   static inline uint generateId( const std::string& typeName );

   // ----------------------------------------------------------------------
   // Comparison
   // ----------------------------------------------------------------------
   
   /**
    * Checks if this type can be downcast onto the specified reference type.
    *
    * @param referenceType
    */
   virtual bool isA( const ReflectionType& referenceType ) const;

   /**
    * Checks if this type can exactly matches the specified reference type.
    *
    * @param referenceType
    */
   inline bool isExactlyA( const ReflectionType& referenceType ) const;
};

///////////////////////////////////////////////////////////////////////////////

struct SerializableTypeInstantiator
{
   virtual ~SerializableTypeInstantiator() {}

   virtual void* instantiate() const = 0;

   virtual void* instantiate( const void* pRhs ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////

template< typename T >
struct TSerializableTypeInstantiator : public SerializableTypeInstantiator
{
   DECLARE_ALLOCATOR( TSerializableTypeInstantiator, AM_DEFAULT );

   void* instantiate() const;
   void* instantiate( const void* pRhs ) const;
};

/**
 * Definition of a type that can be recognized at runtime and can serialize
 * objects defined using it.
 */
class SerializableReflectionType : public ReflectionType
{
   DECLARE_ALLOCATOR( SerializableReflectionType, AM_DEFAULT );

public:
   struct ParentTypeDesc
   {
      uint     m_id;
      int      m_version;

      ParentTypeDesc( uint id, int version ) : m_id( id ), m_version( version ) {}
   };

public:
   SerializableTypeInstantiator*                      m_instantiator;
   std::vector< ReflectionTypeComponent* >            m_memberFields;

   std::vector< ParentTypeDesc >                      m_baseTypesIds;

   // runtime data
   TSContinuousMemoryPool*                            m_searchMemPool;

public:
   /**
    * Constructor.
    *
    * @param name             type name
    */
   SerializableReflectionType( const std::string& name );
   ~SerializableReflectionType();

   // ----------------------------------------------------------------------
   // Instantiation
   // ----------------------------------------------------------------------

   /**
    * Sets up an instantiator of the represented type.
    */
   void setupInstantiator( SerializableTypeInstantiator* instantiator );

   /**
    * Creates an instance of this type.
    */
   template< typename T >
   T* instantiate() const;

   /**
    * Creates a copy of an instance of this type.
    */
   template< typename T >
   T* instantiate( const T& rhs ) const;

   // ----------------------------------------------------------------------
   // Type definition
   // ----------------------------------------------------------------------

   /**
    * Defines a base type of this type ( in order to set up an inheritance hierarchy ).
    *
    * @param baseTypeName
    * @param versionNo           -1 means that this type is based on the latest version of the specified parent type
    */
   void addBaseType( const std::string& baseTypeName, int versionNo = -1 );

   /**
    * Defines a base type of this type ( in order to set up an inheritance hierarchy ).
    *
    * @param baseTypeId
    * @param versionNo           -1 means that this type is based on the latest version of the specified parent type
    */
   void addBaseType( uint baseTypeId, int versionNo = -1 );

   /**
    * Removes the specified base type.
    *
    * @param baseTypeId
    */
   void removeBaseType( uint baseTypeId );

   /**
    * Adds a new type member field.
    *
    * @param member     type member field description
    */
   void addMemberField( ReflectionTypeComponent* member );

   /**
    * Removes an existing member field.
    *
    * @param memberId
    */
   void removeMemberField( uint memberId );

   /**
    * Looks for a member with the specified member id.
    *
    * @param      memberId
    * @return     pointer to the member definition, or NULL if such a member is not defined
    */
   ReflectionTypeComponent* findMemberField( uint memberId ) const;

   /**
    * Looks for a member with the specified member name.
    *
    * @param      memberName
    * @return     pointer to the member definition, or NULL if such a member is not defined
    */
   ReflectionTypeComponent* findMemberField( const std::string& memberName ) const;

   /**
    * Collects member fields that have the specified traits - the method will scan all of type's parents.
    *
    * @param outFields
    * @param traits           fields with what traits do we want
    */
   void collectMemberFields( List< const ReflectionTypeComponent* >& outFields, byte traits ) const;

   /**
    * Creates the properties for this type and the specified object.
    *
    * @param hostObj
    * @param outProperties
    */
   void collectProperties( void* hostObj, std::vector< ReflectionProperty* >& outProperties ) const;

   /**
    * Returns the value stored in the field.
    *
    * @param FieldType
    * @param fieldName
    * @param object        pointer to the memory where the object of this type is located
    * @param outValue
    */
   template< typename FieldType >
   void getFieldValue( const std::string& fieldName, const void* object, FieldType& outValue ) const;

   /**
    * Tells if the type is abstract or not ( can it be instantiated or not ).
    */
   inline bool isAbstract() const;

   /**
    * Collects the parent types of this type.
    *
    * @param outParentTypes
    */
   void collectParents( List< const SerializableReflectionType* >& outParentTypes ) const;

   /**
    * The opposite of the `collectParents` method - collects the types to which
    * this type is a parent.
    *
    * @param outDerivedTypes
    * @param includeAbstractTypes      should abstract implementations be taken into consideration?
    */
   void collectImplementations( std::vector< const SerializableReflectionType* >& outDerivedTypes, bool includeAbstractTypes ) const;

   /**
    * Returns a list of all types present in this type's hierarchy.
    */
   void mapTypesHierarchy( std::list< const SerializableReflectionType* >& outReflectionTypesList ) const;

   // ----------------------------------------------------------------------
   // Serialization support
   // ----------------------------------------------------------------------

   /**
    * Serializes an object representation that corresponds to this type definition.
    *
    * @param object              serialized object
    * @param dependenciesMapper  dependencies (embedded objects) mapper
    * @param stream              output data stream to which the object should be serialized
    */
   template< typename T >
   void save( const T& object, const ReflectionSaver& dependenciesMapper, OutStream& stream ) const;

   /**
    * Deserializes an object representation that corresponds to this type definition.
    *
    * @param stream           input data stream that contains the serialized object
    */
   template< typename T >
   static T* load( InStream& stream );


   // ----------------------------------------------------------------------
   // ReflectionType implementation
   // ----------------------------------------------------------------------
   bool isA( const ReflectionType& referenceType ) const;

private:

   template< typename T >
   void saveMemberFields( const T* object, const ReflectionSaver& dependenciesMapper, SerializedReflectionObject& savedObject ) const;

   inline void loadMemberFields( void* object, const SerializedReflectionObject& savedObject ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Returns an offset of the specified class member.
 */
#define MEMBER_OFFSET( OBJECT, MEMBER_NAME ) (int)( reinterpret_cast< long >( &(OBJECT)->MEMBER_NAME ) )

///////////////////////////////////////////////////////////////////////////////

#include "core/ReflectionType.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_TYPE_H
