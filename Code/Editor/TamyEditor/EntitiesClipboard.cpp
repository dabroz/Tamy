#include "EntitiesClipboard.h"
#include "DebugGeometry.h"
#include "core-MVC\Entity.h"


///////////////////////////////////////////////////////////////////////////////

EntitiesClipboard::~EntitiesClipboard()
{
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesClipboard::copySelectedEntities( const List< SceneNode* >& selectedNodes, Entity* rootEntity )
{
   // delete the clipboard contents
   for ( List< Entity* >::iterator it = m_clipboard.begin(); !it.isEnd(); ++it )
   {
      Entity* entity = *it;
      entity->removeReference();
   }
   m_clipboard.clear();

   // clone selected entities ( root entities only )
   Vector groupCenterWorldSpace;
   if ( selectedNodes.empty() )
   {
      return;
   }

   for ( List< SceneNode* >::const_iterator it = selectedNodes.begin(); !it.isEnd(); ++it )
   {
      SceneNode* selectedNode = *it;
      if ( selectedNode->isA< DebugGeometry >() )
      {
         // we're not cloning debug geometry
         continue;
      }

      if ( selectedNode->getParent() != rootEntity || !selectedNode->isA< Entity >() )
      {
         continue;
      }

      Entity* clonedEntity = static_cast< Entity* >( selectedNode->clone() );
      m_clipboard.pushBack( clonedEntity );

      groupCenterWorldSpace.add( clonedEntity->getLocalMtx().position() );
   }

   // calculate group's center position
   groupCenterWorldSpace.mul( FastFloat::fromFloat( 1.0f / ( float ) selectedNodes.size() ) );

   // translate the selected entities to their group space
   Matrix worldToGroup;
   worldToGroup.setTranslation( groupCenterWorldSpace );
   worldToGroup.invert();
   for ( List< Entity* >::iterator it = m_clipboard.begin(); !it.isEnd(); ++it )
   {
      Entity* entity = *it;
      entity->accessLocalMtx().mul( worldToGroup );
   }
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesClipboard::pasteSelectedEntities( const Vector& instantiationPos, Entity* rootEntity )
{
   Matrix groupTranslationMtx;
   groupTranslationMtx.setTranslation( instantiationPos );

   for ( List< Entity* >::iterator it = m_clipboard.begin(); !it.isEnd(); ++it )
   {
      Entity* entity = *it;
      Entity* clonedEntity = static_cast< Entity* >( entity->clone() );

      clonedEntity->accessLocalMtx().mul( groupTranslationMtx );
      rootEntity->addChild( clonedEntity );
   }
}

///////////////////////////////////////////////////////////////////////////////
