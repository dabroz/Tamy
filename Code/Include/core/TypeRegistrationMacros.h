/// @file   core/TypeRegistrationMacros.h
/// @brief  macros you should use to register the reflection types
#pragma once

#include "core/ReflectionObject.h"
#include "core/ResourceHandle.h"
#include "core/RefPtr.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Reflection type registration macros.
 */
#ifndef IMPORT_RTTI_REGISTRATIONS
   #define REGISTER_TYPE( ClassType )
   #define REGISTER_ABSTRACT_TYPE( ClassType )
   #define REGISTER_EXTERNAL_TYPE( ClassType )
   #define REGISTER_ENUM_TYPE( ClassType )
   #define REGISTER_CORE_TYPES()
#else
   #define REGISTER_TYPE( ClassType )                                         \
   struct ClassType##RTTIImporter                                             \
   {                                                                          \
      ClassType##RTTIImporter()                                               \
      {                                                                       \
         ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance(); \
         typesRegistry.addSerializableType< ClassType >( #ClassType, new TSerializableTypeInstantiator< ClassType >() );  \
      }                                                                       \
   } registerClass##ClassType;                                                \
   DEFINE_TYPE_ID( ClassType );                                               \
   DEFINE_TYPE_ID( TResourceHandle< ClassType > );                            \
   DEFINE_TYPE_ID( TRefPtr< ClassType > );                                    \
   DEFINE_TYPE_ID( TShared< ClassType > );

   #define REGISTER_ABSTRACT_TYPE( ClassType )                                \
   struct ClassType##RTTIImporter                                             \
   {                                                                          \
      ClassType##RTTIImporter()                                               \
      {                                                                       \
         ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance(); \
         typesRegistry.addSerializableType< ClassType >( #ClassType, NULL ); \
      }                                                                       \
   } registerClass##ClassType;                                                \
   DEFINE_TYPE_ID( ClassType );                                               \
   DEFINE_TYPE_ID( TResourceHandle< ClassType > );                            \
   DEFINE_TYPE_ID( TRefPtr< ClassType > );                                    \
   DEFINE_TYPE_ID( TShared< ClassType > );

   #define REGISTER_EXTERNAL_TYPE( ClassType )                                \
   struct ClassType##RTTIImporter                                             \
   {                                                                          \
      ClassType##RTTIImporter()                                               \
      {                                                                       \
         ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance(); \
         typesRegistry.addExternalType< ClassType >( #ClassType );            \
      }                                                                       \
   } registerClass##ClassType;                                                \
   DEFINE_TYPE_ID( ClassType );
   // An external type can't possibly inherit from Resource, thus it can only be a resource-internal object.
   // for that reason we're not registering a TResourceHandle< ClassType > type id.
   // Same principle applies to an enum type defined below

   #define REGISTER_ENUM_TYPE( EnumType )                                     \
   struct EnumType##RTTIImporter                                              \
   {                                                                          \
      EnumType##RTTIImporter()                                                \
      {                                                                       \
         ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance(); \
         typesRegistry.addEnumType< EnumType >( #EnumType );                  \
      }                                                                       \
   } registerClass##EnumType;                                                 \
   DEFINE_TYPE_ID( EnumType );

   #define REGISTER_CORE_TYPES()                                              \
   struct __CoreTypesRTTIImporter                                             \
   {                                                                          \
      __CoreTypesRTTIImporter()                                               \
      {                                                                       \
         ReflectionTypesRegistry& typesRegistry = TSingleton< ReflectionTypesRegistry >::getInstance(); \
         typesRegistry.addExternalType< int >( "int" );                       \
         typesRegistry.addExternalType< unsigned int >( "unsigned int" );     \
         typesRegistry.addExternalType< char >( "char" );                     \
         typesRegistry.addExternalType< unsigned char >( "unsigned char" );   \
         typesRegistry.addExternalType< long >( "long" );                     \
         typesRegistry.addExternalType< unsigned long >( "unsigned long" );   \
         typesRegistry.addExternalType< short >( "short" );                   \
         typesRegistry.addExternalType< unsigned short >( "unsigned short" ); \
         typesRegistry.addExternalType< bool >( "bool" );                     \
         typesRegistry.addExternalType< float >( "float" );                   \
         typesRegistry.addExternalType< double >( "double" );                 \
         typesRegistry.addExternalType< std::string >( "std::string" );       \
         typesRegistry.addSerializableType< ReflectionObject >( "ReflectionObject", NULL ) ; \
      }                                                                       \
   } registerClass##EnumType;                                                 \
   DEFINE_TYPE_ID( int )                                                      \
   DEFINE_TYPE_ID( unsigned int )                                             \
   DEFINE_TYPE_ID( char )                                                     \
   DEFINE_TYPE_ID( unsigned char )                                            \
   DEFINE_TYPE_ID( long )                                                     \
   DEFINE_TYPE_ID( unsigned long )                                            \
   DEFINE_TYPE_ID( short )                                                    \
   DEFINE_TYPE_ID( unsigned short )                                           \
   DEFINE_TYPE_ID( bool )                                                     \
   DEFINE_TYPE_ID( float )                                                    \
   DEFINE_TYPE_ID( double )                                                   \
   DEFINE_TYPE_ID( std::string )                                              \
   DEFINE_TYPE_ID( ReflectionEnum )                                           \
   DEFINE_TYPE_ID( ReflectionObject )														\
   DEFINE_TYPE_ID( TResourceHandle< ReflectionObject > )

#endif

///////////////////////////////////////////////////////////////////////////////
