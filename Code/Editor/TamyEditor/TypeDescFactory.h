/// @file   TamyEditor/TypeDescFactory.h
/// @brief  Factory creating descriptions of types.
#ifndef _TYPE_DESC_FACTORY_H
#define _TYPE_DESC_FACTORY_H

#include "TreeWidgetDescFactory.h"
#include "core.h"


///////////////////////////////////////////////////////////////////////////////

class Filesystem;

///////////////////////////////////////////////////////////////////////////////

/**
 * The factory provides a description of the specified type descendants
 * in the form of a name and an icon.
 *
 * It can be used with a TreeWidget as the items descriptions factory.
 */
template< typename T >
class TypeDescFactory : public TreeWidgetDescFactory
{
private:
   const Filesystem&                                     m_fs;
   QString                                               m_defaultIcon;
   std::vector< const SerializableReflectionType* >      m_classes;

public:
   /**
    * Constructor.
    *
    * @param defaultIcon   icon used when no other matching icon is found
    */
   TypeDescFactory( const QString& defaultIcon );

   /**
    * Removes a specific type from the list of created types.
    *
    * @param TypeToRemove
    */
   template< typename TypeToRemove >
   void removeType();

   // -------------------------------------------------------------------------
   // TreeWidgetDescFactory implementation
   // -------------------------------------------------------------------------
   unsigned int typesCount() const;
   void getDesc( unsigned int idx, QString& outDesc, QString& outGroup, QIcon& outIcon ) const;
   void getDesc( const SerializableReflectionType& type, QString& outDesc, QString& outGroup, QIcon& outIcon ) const;
   const SerializableReflectionType* getClass( unsigned int idx ) const;

private:
   QString getIconName( const SerializableReflectionType& type ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "TypeDescFactory.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _TYPE_DESC_FACTORY_H
