#include "StoryBrowserTreeItem.h"
#include "ext-StoryTeller\StoryNode.h"
#include "ext-StoryTeller\StoryNodeInstance.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include "core-MVC\Model.h"
#include "core\ReflectionProperty.h"


///////////////////////////////////////////////////////////////////////////////

StoryBrowserTreeItem::StoryBrowserTreeItem()
   : m_previewEnabled( false )
   , m_previewFrozen( false )
   , m_snapshotEntity( NULL )
   , m_snapshotScene( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryBrowserTreeItem::~StoryBrowserTreeItem()
{
   ASSERT( !m_snapshotScene && !m_snapshotEntity );
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeItem::initialize( Model& scene, StoryBrowserTreeItem* parentItem )
{
   m_snapshotScene = &scene;

   if ( parentItem )
   {
      m_previewFrozen = parentItem->m_previewFrozen;

      if ( parentItem->m_previewEnabled )
      {
         toggleSnapshot( false );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeItem::clearSnapshot()
{
   if ( m_snapshotEntity )
   {
      ASSERT( m_snapshotScene );

      // remove spawned snapshot entity
      m_snapshotEntity->remove();
      m_snapshotEntity->removeReference();
      m_snapshotEntity = NULL;
   }

   m_snapshotScene = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeItem::updateDescription()
{
   StoryNode* node = getNode();
   setText( 0, node->getName().c_str() );

   if ( m_previewEnabled && !m_previewFrozen )
   {
      // update the snapshot, providing it's changed
      Prefab* previousPrefab = m_snapshotEntity ? m_snapshotEntity->m_prefab : NULL;
      Prefab* newPrefab = node->getRepresentationPrefab();
      if ( previousPrefab != newPrefab )
      {
         // destroy old entity
         if ( m_snapshotEntity )
         {
            m_snapshotEntity->remove();
            m_snapshotEntity->removeReference();
            m_snapshotEntity = NULL;
         }

         // create a new entity
         m_snapshotEntity = getNode()->instantiate();
         if ( m_snapshotEntity )
         {
            m_snapshotEntity->addReference();
            m_snapshotScene->addChild( m_snapshotEntity );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeItem::toggleSnapshot( bool first )
{
   if ( isContainerNode() && !first )
   {
      // we're only showing objects from the first chapter
      return;
   }

   m_previewEnabled = !m_previewEnabled; 

   // instantiate/destroy the entities from this chapter, but only if the preview is not frozen.
   // If it's frozen - do all 'editor' things, like updating icons etc, but don't spawn/despawn snapshot 
   // items
   updatePreviewEntity();

   int count = childCount();
   for ( int i = 0; i < count; ++i )
   {
      StoryBrowserTreeItem* childItem = static_cast< StoryBrowserTreeItem* >( child(i) );
      childItem->toggleSnapshot( false );
   }

   // toggle the icon that shows whether we're previewing the chapter's contents or not
   if ( m_previewEnabled )
   {
      setIcon( 1, QIcon( ":/TamyEditor/Resources/Icons/Editor/preview.png" ) );
   }
   else
   {
      setIcon( 1, QIcon() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeItem::freezePreview( bool freeze )
{
   m_previewFrozen = freeze;

   updatePreviewEntity();
}

///////////////////////////////////////////////////////////////////////////////

void StoryBrowserTreeItem::updatePreviewEntity()
{
   bool spawn = m_previewEnabled && !m_previewFrozen;

   if ( spawn )
   {
      ASSERT( m_snapshotEntity == NULL );
      m_snapshotEntity = getNode()->instantiate();
      if ( m_snapshotEntity )
      {
         m_snapshotEntity->addReference();
         m_snapshotScene->addChild( m_snapshotEntity );
      }
   }
   else
   {
      if ( m_snapshotEntity )
      {
         m_snapshotEntity->remove();
         m_snapshotEntity->removeReference();
         m_snapshotEntity = NULL;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
