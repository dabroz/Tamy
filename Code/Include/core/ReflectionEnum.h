/// @file   core/ReflectionEnum.h
/// @brief  serializable enums convertible to strings.
//          NOTE: The credit for this code goes to Francis Xavier Joseph Pulikotil, based on
//          whose code this implementation was derived
#ifndef _REFLECTION_ENUM_H
#define _REFLECTION_ENUM_H

#include <string>
#include <map>
#include <vector>
#include "core/MemoryRouter.h"
#include "core/ReflectionType.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * An abstract enum RTTI type.
 */
class ReflectionEnum : public ReflectionType
{
   DECLARE_ALLOCATOR( ReflectionEnum, AM_DEFAULT );

public:
   /**
    * Constructor.
    *
    * @param typeName
    */
   ReflectionEnum( const std::string& typeName );
   virtual ~ReflectionEnum() {}

   /**
    * Returns all enumerators the enumeration type has.
    *
    * @param outValues     array which upon return will be filled with enumerators
    */
   virtual void getEnumerators( std::vector< std::string >& outEnumerators ) const {}

   /**
    * Returns an index of the specified enumerator value in the enumerators array.
    *
    * @param enumeratorValue
    */
   virtual int getIndex( int enumeratorValue ) const { return -1; }

   /**
    * Returns a value assigned to the specified enumerator.
    *
    * @param enumeratorName
    */
   virtual int getValue( const std::string& enumeratorName ) const { return -1; }

   // -------------------------------------------------------------------------
   // ReflectionType implementation
   // -------------------------------------------------------------------------
   bool isA( const ReflectionType& referenceType ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Serializable enums base class/
 */ 
template< typename EnumType >
class TReflectionEnum : public ReflectionEnum
{
protected:
   typedef std::map< typename EnumType, std::string > AssocMap;
   AssocMap                                  m_assocMap;

public:
   /**
    * Constructor.
    */
   explicit TReflectionEnum();

   /**
    * Constructor.
    *
    * @param typeName
    */
   explicit TReflectionEnum( const std::string& typeName );
   ~TReflectionEnum() {}

   /**
    * Converts from an enumerator to a string.
    *
    * @param enumerator
    * @return              a string representation of an enumerator, 
    *                      or an empty string if the enumerator was not registered.
    */
   const std::string& from( const EnumType enumerator ) const;

   /**
    * Converts from a string to an enumerator.
    *
    * @param outEnumerator    enumerator
    * @param str              string version of the enumerator
    * @return                 'true' if the conversion is successful, 'false' otherwise.
    */
   const bool to( const std::string& str, EnumType& outEnumerator ) const;

   /**
    * Returns the map of registered enums.
    */
   inline AssocMap &getMap();

   /**
    * Registers the enumerators the enum has.
    */
   void registerEnumerators();

   // -------------------------------------------------------------------------
   // ReflectionType implementation
   // -------------------------------------------------------------------------
   bool isA( const ReflectionType& referenceType ) const;

   // -------------------------------------------------------------------------
   // ReflectionEnum implementation
   // -------------------------------------------------------------------------
   void getEnumerators( std::vector< std::string >& outEnumerators ) const;
   int getIndex( int enumeratorValue ) const;
   int getValue( const std::string& enumeratorName ) const;

protected:
   /**
    * Use this helper function to register each enumerator
    * and its string representation.
    *
    * @param enumerator
    * @param eStr          string representation of the enumerator
    */
   void registerEnumerator( const EnumType enumerator, const std::string &eStr );

private:
   /**
    * Copy constructor.
    */
   TReflectionEnum( const TReflectionEnum& );

   /**
    * Assignment operator.
    */
   const TReflectionEnum& operator=( const TReflectionEnum& );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Begins an enum definition.
 */
#define BEGIN_ENUM( EnumName )                                                \
DEFINE_DEFAULT_APPROXIMATE_EQUALITY( EnumName );                              \
template<> void TReflectionEnum< EnumName >::registerEnumerators()            \
{

///////////////////////////////////////////////////////////////////////////////

/**
 * Defines an enum value
 */
#define ENUM_VAL( value ) registerEnumerator( value, #value );

///////////////////////////////////////////////////////////////////////////////

/**
 * Begins an enum definition.
 */
#define END_ENUM() }

///////////////////////////////////////////////////////////////////////////////

#include "core/ReflectionEnum.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_ENUM_H
