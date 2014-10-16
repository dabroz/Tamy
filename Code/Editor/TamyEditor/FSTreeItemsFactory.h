/// @file   TamyEditor/FSTreeItemsFactory.h
/// @brief  items factory that creates new items that can populate the filesystem tree
#pragma once

#include "TreeWidgetDescFactory.h"
#include "TypeDescFactory.h"
#include <QString>


///////////////////////////////////////////////////////////////////////////////

class Resource;

///////////////////////////////////////////////////////////////////////////////

enum FSTreeItemType
{
   FS_ITEM_DIR,
   FS_ITEM_PROJECT,
};

///////////////////////////////////////////////////////////////////////////////

class FSTreeItemsFactory : public TreeWidgetDescFactory
{
private:
   TypeDescFactory< Resource >*  m_resourcesFactory;

public:
   /**
    * Constructor.
    */ 
   FSTreeItemsFactory();
   ~FSTreeItemsFactory();

   // -------------------------------------------------------------------------
   // TreeWidgetDescFactory implementation
   // -------------------------------------------------------------------------
   unsigned int typesCount() const;
   void getDesc( unsigned int idx, QString& outDesc, QString& outGroup, QIcon& outIcon ) const;
   void getDesc( const SerializableReflectionType& type, QString& outDesc, QString& outGroup, QIcon& outIcon ) const;
   const SerializableReflectionType* getClass( unsigned int idx ) const;

};

///////////////////////////////////////////////////////////////////////////////
