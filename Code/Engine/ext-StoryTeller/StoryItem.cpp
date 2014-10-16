#include "ext-StoryTeller\StoryItem.h"
#include "ext-StoryTeller\Story.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "core-MVC\Prefab.h"
#include "core\Assert.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( StoryItem );
   PARENT( StoryNode );
   PROPERTY_EDIT( "Thumbnail", FilePath, m_thumbnailPath );
   PROPERTY_EDIT( "Is unique", bool, m_uniqueItem );
   PROPERTY_EDIT( "Transform", Matrix, m_transform );
   PROPERTY_EDIT( "Item prefab", Prefab*, m_itemPrefab );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

StoryItem::StoryItem()
   : m_uniqueItem( false )
   , m_itemPrefab( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryItem::~StoryItem()
{
   // item model resource will be removed by the resources manager
   m_itemPrefab = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryItem::setTransform( const Matrix& transform )
{
   m_transform = transform;
}

///////////////////////////////////////////////////////////////////////////////

void StoryItem::setItemPrefab( Prefab* itemPrefab )
{
   m_itemPrefab = itemPrefab;

   // notify object's listeners about the property change
   {
      ReflectionProperty* property = getProperty( "m_itemPrefab" );
      notifyPropertyChange( *property );
      delete property;
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryItem::pullStructure( StoryListener* listener )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void StoryItem::onHostStorySet( Story* story )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

StoryNodeInstance* StoryItem::instantiate()
{
   StoryNodeInstance* itemModelEntity = NULL;

   if ( m_itemPrefab )
   {
      itemModelEntity = new StoryNodeInstance( m_itemPrefab, this, getName().c_str() );

      // set the local transform and commit it to the global transform - children components,
      // such as the GL2DBody, will refer to that one when controlling the position of the body
      itemModelEntity->setLocalMtx( m_transform );
      itemModelEntity->updateTransforms();

      itemModelEntity->attachListener( *this );
   }

   return itemModelEntity;
}

///////////////////////////////////////////////////////////////////////////////

void StoryItem::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_localMtx" )
   {
      // item instance was moved - copy its transform
      Matrix* itemLocalMtx = (Matrix*)property.edit();
      if ( m_transform != *itemLocalMtx )
      {
         m_transform = *itemLocalMtx;

         ReflectionProperty* property = getProperty( "m_transform" );
         notifyPropertyChange( *property );
         delete property;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryItem::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
}

///////////////////////////////////////////////////////////////////////////////
