/// @file   core\ReflectionTypeID.h
/// @brief  utility for deducing names of types.
#ifndef REFLECTION_TYPE_ID_H
#define REFLECTION_TYPE_ID_H

#include "core\MemoryRouter.h"
#include "core\StringUtils.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

/**
 * Utility for deducing names of types.
 */
template< typename T >
struct ReflectionTypeID
{
   DECLARE_ALLOCATOR( ReflectionTypeID, AM_DEFAULT );

   std::string    m_name;
   unsigned int   m_id;

   /**
    * Constructor for regular types.
    *
    * @param T     type we want to deduce
    */
   ReflectionTypeID();
};

///////////////////////////////////////////////////////////////////////////////

#ifndef IMPORT_RTTI_REGISTRATIONS
   #define DEFINE_TYPE_ID( Type )
#else
   #define DEFINE_TYPE_ID( Type )                                             \
   template<>                                                                 \
   ReflectionTypeID< Type >::ReflectionTypeID()                               \
   {                                                                          \
      m_name = #Type;                                                         \
      m_id = StringUtils::calculateHash( m_name );                            \
   }                                                                          \
   template<>                                                                 \
   ReflectionTypeID< std::vector< Type > >::ReflectionTypeID()                \
   {                                                                          \
      m_name = "__Vector__"#Type;                                             \
      m_id = StringUtils::calculateHash( m_name );                            \
   }                                                                          \
   template<>                                                                 \
   ReflectionTypeID< std::vector< Type* > >::ReflectionTypeID()               \
   {                                                                          \
      m_name = "__VectorPtr__"#Type;                                          \
      m_id = StringUtils::calculateHash( m_name );                            \
   }
#endif

///////////////////////////////////////////////////////////////////////////////

/**
 * Use this macro if you want to map a type to another one.
 * This can happen i.e. if your class hierarchy looks like so:
 *
 *   ReflectionObject <--- template class Interface <---- class SpecifiedImplementation : public Interface< XXX >
 *
 * where: 
 *   - Interface						is a template class that cannot be reflected upon until it's specified
 *   - SpecifiedImplementation	is one of the specified implementations of the interface that can be reflected upon
 *
 * now if for some reason we would like to use the Interface< XXX >* as a type ( as a collection member perhaps, which is 
 * very common ), and then reflect upon that, the type would be unrecognized since only ReflectionObject and the SpecifiedImplementation
 * have their types defined.
 * That's where this macro comes in - you can define a mapping, that whenever Interface< XXX >is encountered, another ( preferably its base ) type
 * will be returned.
 *
 * In this case, all you need to do is set it up like so:
 *
 *   MAP_TYPE( Interface< XXX >, ReflectionObject )
 *
 * and henceforth every typeid operation on this type will return the type of ReflectionObject
 */
#ifndef IMPORT_RTTI_REGISTRATIONS
   #define MAP_TYPE( MappedType, ExistingType )
#else
   #define MAP_TYPE( MappedType, ExistingType )                                \
   template<>                                                                  \
   ReflectionTypeID< MappedType >::ReflectionTypeID()                          \
   {                                                                           \
      m_name = #ExistingType;                                                  \
      m_id = StringUtils::calculateHash( m_name );                             \
   }                                                                           \
   template<>                                                                  \
   ReflectionTypeID< std::vector< MappedType > >::ReflectionTypeID()           \
   {                                                                           \
      m_name = "__Vector__"#ExistingType;                                      \
      m_id = StringUtils::calculateHash( m_name );                             \
   }                                                                           \
   template<>                                                                  \
   ReflectionTypeID< std::vector< MappedType* > >::ReflectionTypeID()          \
   {                                                                           \
      m_name = "__VectorPtr__"#ExistingType;                                   \
      m_id = StringUtils::calculateHash( m_name );                             \
   }

#endif 

///////////////////////////////////////////////////////////////////////////////

#include "core\ReflectionTypeID.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // REFLECTION_TYPE_ID_H
