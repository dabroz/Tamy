#include "SelectionManager.h"
#include "core-MVC\Model.h"
#include "core-MVC\Entity.h"
#include "core-MVC\EntitiesGroup.h"
#include "core-Renderer\RendererSelectionListener.h"
#include "SelectionManagerListener.h"
#include "DebugGeometry.h"
#include "DebugEntitiesManager.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

SelectionManager::SelectionManager( EntitiesGroup& selectedEntities )
   : m_appendedSelectionEnabled( false )
   , m_selectedEntities( selectedEntities )
{
}

///////////////////////////////////////////////////////////////////////////////

SelectionManager::~SelectionManager()
{
   m_selectedEntities.clear();
   m_selectedNodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::attach( SelectionManagerListener& listener )
{
   Listeners::iterator it = ListUtils::find( m_listeners, &listener );
   if ( it.isEnd() )
   {
      m_listeners.pushBack( &listener );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::detach( SelectionManagerListener& listener )
{
   Listeners::iterator it = ListUtils::find( m_listeners, &listener );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::notifySelectionChanged()
{
   for ( Listeners::iterator it = m_listeners.begin(); !it.isEnd(); ++it )
   {
      SelectionManagerListener* listener = *it;
      listener->onSelectionChanged( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::onAttachedToModel( Model& model )
{
   ModelView::onAttachedToModel( model );

   model.pullStructure( this );
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::onNodeAdded( SceneNode* node )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::onNodeRemoved( SceneNode* node )
{
   List< SceneNode* >::iterator nodeIt = ListUtils::find( m_selectedNodes, node );
   if ( !nodeIt.isEnd() )
   {
      nodeIt.markForRemoval();
      nodeIt.processElementsRemoval();

      if ( node->isA< Entity >() )
      {
         Entity* removedEntity = static_cast< Entity* >( node );
         m_selectedEntities.removeEntity( removedEntity );
      }

      notifySelectionChanged();
   }
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::onNodeChanged( SceneNode* node )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::resetContents( Model& scene )
{
   m_selectedEntities.clear();
   m_selectedNodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::selectNodesFromViewport( const List< SceneNode* >& nodes )
{
   // some debug entities might have been selected - extract pointers to scene entities from them
   List< SceneNode* > sceneNodes;
   for ( List< SceneNode* >::const_iterator it = nodes.begin(); !it.isEnd(); ++it )
   {
      SceneNode* node = *it;
      if ( !node )
      {
         return;
      }
      
      if ( node->isA< DebugGeometry >() )
      {
         DebugGeometry* debugGeom = static_cast< DebugGeometry* >( node );
         SceneNode* representedEntity = debugGeom->getRepresentedNodePtr();
         if ( representedEntity )
         {
            sceneNodes.pushBack( representedEntity );
         }
      }
      else
      {
         sceneNodes.pushBack( node );
      }
   }

   // change selection
   if ( m_appendedSelectionEnabled )
   {
      List< SceneNode* > selectedNodes;
      List< SceneNode* > deselectedNodes;
      ListUtils::compareWithPrevious( m_selectedNodes, sceneNodes, selectedNodes, deselectedNodes );

      for ( List< SceneNode* >::iterator it = selectedNodes.begin(); !it.isEnd(); ++it )
      {
         SceneNode* selNode = *it;
         m_selectedNodes.pushBack( selNode );
         
         if ( selNode->isA< Entity >() )
         {
            m_selectedEntities.addEntity( static_cast< Entity* >( selNode ) );
         }
      }
   }
   else
   {
      m_selectedNodes = sceneNodes;

      // update the selection group
      m_selectedEntities.clear();
      for ( List< SceneNode* >::iterator it = m_selectedNodes.begin(); !it.isEnd(); ++it )
      {
         SceneNode* selNode = *it;
         if ( selNode->isA< Entity >() )
         {
            m_selectedEntities.addEntity( static_cast< Entity* >( selNode ) );
         }
      }
   }

   // notify listeners
   notifySelectionChanged();
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::selectNode( SceneNode& node )
{
   if ( !m_appendedSelectionEnabled )
   {
      m_selectedNodes.clear();
      m_selectedEntities.clear();
   }

   // if we selected debug geometry, access the entity it represents and select it instead
   if ( node.isA< DebugGeometry >() )
   {
      DebugGeometry* debugGeom = static_cast< DebugGeometry* >( &node );
      SceneNode* representedSceneNode = debugGeom->getRepresentedNodePtr();

      if ( representedSceneNode->isA< Entity >() )
      {
         m_selectedEntities.addEntity( static_cast< Entity* >( representedSceneNode ) );
      }
      m_selectedNodes.pushBack( representedSceneNode );
   }
   else
   {
      m_selectedNodes.pushBack( &node );

      if ( node.isA< Entity >() )
      {
         m_selectedEntities.addEntity( static_cast< Entity* >( &node ) );
      }
   }

   // notify listeners
   notifySelectionChanged();
}

///////////////////////////////////////////////////////////////////////////////

void SelectionManager::resetSelection()
{
   m_selectedEntities.clear();
   m_selectedNodes.clear();

   // notify listeners
   notifySelectionChanged();
}

///////////////////////////////////////////////////////////////////////////////
