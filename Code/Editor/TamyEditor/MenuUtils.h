/// @file   TamyEditor/MenuUtils.h
/// @brief  a set of utilities for building various menus used throughout the editor
#ifndef _MENU_UTILS_H
#define _MENU_UTILS_H


///////////////////////////////////////////////////////////////////////////////

class TreeWidgetDescFactory;
class QMenu;
class MenuActionsFactory;
class QWidget;
class QIcon;
class QString;
class QAction;
class SerializableReflectionType;

///////////////////////////////////////////////////////////////////////////////

enum ItemIconState
{
   Icon_Regular,
   Icon_Active
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A set of utilities for building various menus used throughout the editor.
 */
class MenuUtils
{
public:
   /**
    * Creates a menu that lists all items the specified factory can create and assigns a custom action to them.
    *
    * The items will be divided into groups. The groups are defined in "Settings.ini" file under [TypeGroups] group.
    *
    * @param itemsFactory     factory of items which we want to list
    * @param actionsFactory   factory of menu actions
    * @param hostMenu         menu to which the entries will be added
    */
   static void itemsListMenu( TreeWidgetDescFactory* itemsFactory, MenuActionsFactory& actionsFactory, QMenu* hostMenu );

   /**
    * Returns the name of an icon for the specified type.
    * In addition, you can specify the state the icon should reflect - some icons can illustrate
    * that an item wearing it is active for instance.
    *
    * @param type
    * @param state
    */
   static QString getIconForType( const SerializableReflectionType& type, ItemIconState state = Icon_Regular );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A factory that creates menu actions. Used by various MenuUtils utility methods.
 */
class MenuActionsFactory
{
public:
   /**
    * Creates a menu action.
    *
    * @param icon
    * @param desc
    * @param itemTypeIdx
    * @param parentWidget
    */
   virtual QAction* create( const QIcon& icon, const QString& desc, int itemTypeIdx, QWidget* parentWidget ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////

#include "MenuUtils.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _MENU_UTILS_H
