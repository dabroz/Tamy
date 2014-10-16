#include "MenuUtils.h"
#include "TreeWidgetDescFactory.h"
#include <QtGui\QIcon>
#include <QtWidgets\QMenu>
#include <QtWidgets\QAction>
#include "core\LocalList.h"
#include "core\ReflectionType.h"
#include <QtCore\QResource>


///////////////////////////////////////////////////////////////////////////////

void MenuUtils::itemsListMenu( TreeWidgetDescFactory* itemsFactory, MenuActionsFactory& actionsFactory, QMenu* hostMenu )
{
   if ( !itemsFactory || itemsFactory->typesCount() <= 0)
   {
      return;
   }


   QString  desc;
   QString  group;
   QIcon    icon;

   typedef std::map< QString, QMenu* > GroupMenusMap;
   GroupMenusMap   groups;

   unsigned int factoriesCount = itemsFactory->typesCount();
   for ( unsigned int itemTypeIdx = 0; itemTypeIdx < factoriesCount; ++itemTypeIdx )
   {
      itemsFactory->getDesc( itemTypeIdx, desc, group, icon );

      // find a proper group for this item ( or create a new one )
      QMenu* groupMenu = NULL;
      if ( group.isEmpty() )
      {
         // if no group name was specified, simply aggregate the entries directly under the host menu
         groupMenu = hostMenu;
      }
      else
      {
         GroupMenusMap::iterator groupIt = groups.find( group );
         if ( groupIt != groups.end() )
         {
            groupMenu = groupIt->second;
         }
         else
         {
            groupMenu = new QMenu( group, hostMenu );
            groups.insert( std::make_pair( group, groupMenu ) );
         }
      }

      QAction* addAction = actionsFactory.create( icon, desc, itemTypeIdx, hostMenu );
      groupMenu->addAction( addAction );
   }

   // now add all the groups to the menu in alphabetical order
   for ( GroupMenusMap::iterator it = groups.begin(); it != groups.end(); ++it )
   {
      hostMenu->addMenu( it->second );
   }
}

///////////////////////////////////////////////////////////////////////////////

QString MenuUtils::getIconForType( const SerializableReflectionType& type, ItemIconState state )
{
   // find the first applicable icon there is for this type
   LocalList< const SerializableReflectionType* > typesQueue;
   typesQueue.pushBack( &type );
   QString validIconName;
   while ( !typesQueue.empty() )
   {
      const SerializableReflectionType& currType = *typesQueue.front();
      QString iconName;
      
      switch( state )
      {
      case Icon_Active:
         {
            iconName = QString( ":/TamyEditor/Resources/Icons/Engine/" ) + currType.m_name.c_str() + "Icon_Active.png";
            break;
         }

      case Icon_Regular:
      default:
         {
            iconName = QString( ":/TamyEditor/Resources/Icons/Engine/" ) + currType.m_name.c_str() + "Icon.png";
            break;
         }
      }

      QResource iconRes( iconName );
      if ( iconRes.isValid() )
      {
         validIconName = iconName;
         break;
      }

      currType.collectParents( typesQueue );

      typesQueue.popFront();
   }

   return validIconName;
}

///////////////////////////////////////////////////////////////////////////////
