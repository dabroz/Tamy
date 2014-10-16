#ifndef _TYPE_DESC_FACTORY_H
#error "This file can only be included from TypeDescFactory.h"
#else

#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include "tamyeditor.h"
#include "MenuUtils.h"
#include <QIcon.h>
#include <QtCore\QSettings>
#include <QString>


///////////////////////////////////////////////////////////////////////////////

template< typename T >
TypeDescFactory< T >::TypeDescFactory( const QString& defaultIcon ) 
: m_fs( TSingleton< ResourcesManager >::getInstance().getFilesystem() )
, m_defaultIcon( defaultIcon )
{
   ReflectionTypesRegistry& typesReg = TSingleton< ReflectionTypesRegistry >::getInstance();
   typesReg.getMatchingSerializableTypes< T >( m_classes, false );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T > template< typename TypeToRemove >
void TypeDescFactory< T >::removeType()
{
    const SerializableReflectionType& type = TypeToRemove::getStaticRTTI();

    uint count = m_classes.size();
    for ( uint i = 0; i < count; ++i )
    {
       if ( m_classes[i]->isExactlyA( type ) )
       {
          // found it - remove it and exit
          m_classes.erase( m_classes.begin() + i );
          break;
       }
    }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int TypeDescFactory< T >::typesCount() const 
{
   return m_classes.size();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TypeDescFactory< T >::getDesc( unsigned int idx,  QString& outDesc, QString& outGroup, QIcon& outIcon ) const
{
   const SerializableReflectionType& type = *m_classes[ idx ];
   getDesc( type, outDesc, outGroup, outIcon );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TypeDescFactory< T >::getDesc( const SerializableReflectionType& type, QString& outDesc, QString& outGroup, QIcon& outIcon ) const
{
   // get the name of the type
   outDesc = type.m_name.c_str();

   // find the first applicable icon there is for this type
   QString validIconName = MenuUtils::getIconForType( type );
   if ( validIconName.isEmpty() )
   {
      outIcon = QIcon( QString( ":/TamyEditor/Resources/Icons/Engine/" ) + m_defaultIcon );
   }
   else
   {
      outIcon = QIcon( validIconName );
   }

   // find the group for this type - these are stored in the Settings.ini
   {
      TamyEditor& tamyEd = TamyEditor::getInstance();
      QSettings& edSettings = tamyEd.getSettings();
      edSettings.beginGroup( "TypeGroups" );

      outGroup = edSettings.value( outDesc ).toString();

      edSettings.endGroup();
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const SerializableReflectionType* TypeDescFactory< T >::getClass( unsigned int idx ) const
{
   return m_classes[ idx ];
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
QString TypeDescFactory< T >::getIconName( const SerializableReflectionType& type ) const 
{
   return QString( ":/TamyEditor/Resources/Icons/Engine/" ) + type.m_name.c_str() + "Icon.png";
}

///////////////////////////////////////////////////////////////////////////////

#endif // _TYPE_DESC_FACTORY_H
