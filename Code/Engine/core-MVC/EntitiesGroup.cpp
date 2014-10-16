#include "core-MVC\EntitiesGroup.h"
#include "core-MVC\EntitiesGroupView.h"
#include "core-MVC\Entity.h"
#include "core\ListUtils.h"
#include "core\LocalStack.h"


///////////////////////////////////////////////////////////////////////////////

EntitiesGroup::EntitiesGroup()
{
}

///////////////////////////////////////////////////////////////////////////////

EntitiesGroup::~EntitiesGroup()
{
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesGroup::addEntity( Entity* entity )
{
   List< Entity* >::iterator entityIt = ListUtils::find( m_entities, entity );
   if ( entityIt.isEnd() )
   {
      m_entities.pushBack( entity );

      // notify views
      for ( List< EntitiesGroupView* >::iterator viewIt = m_views.begin(); !viewIt.isEnd(); ++viewIt )
      {
         EntitiesGroupView* view = *viewIt;
         view->onEntityAdded( entity );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesGroup::removeEntity( Entity* entity )
{
   List< Entity* >::iterator entityIt = ListUtils::find( m_entities, entity );
   if ( !entityIt.isEnd() )
   {
      // notify views
      for ( List< EntitiesGroupView* >::iterator viewIt = m_views.begin(); !viewIt.isEnd(); ++viewIt )
      {
         EntitiesGroupView* view = *viewIt;
         view->onEntityRemoved( entity );
      }

      entityIt.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesGroup::clear()
{
   for ( List< Entity* >::iterator entityIt = m_entities.begin(); !entityIt.isEnd(); ++entityIt )
   {
      Entity* entity = *entityIt;

      // notify views
      for ( List< EntitiesGroupView* >::iterator viewIt = m_views.begin(); !viewIt.isEnd(); ++viewIt )
      {
         EntitiesGroupView* view = *viewIt;
         view->onEntityRemoved( entity );
      }
   }
   m_entities.clear();
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesGroup::attachListener( EntitiesGroupView* view )
{
   List< EntitiesGroupView* >::iterator it = ListUtils::find( m_views, view );
   if ( it.isEnd() )
   {
      m_views.pushBack( view );
      view->onAttachedToGroup( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesGroup::detachListener( EntitiesGroupView* view )
{
   List< EntitiesGroupView* >::iterator it = ListUtils::find( m_views, view );
   if ( !it.isEnd() )
   {
      view->onDetachedFromGroup( *this );
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesGroup::pullStructure( EntitiesGroupView* view )
{
   for ( List< Entity* > ::iterator it = m_entities.begin(); !it.isEnd(); ++it )
   {
      Entity* entity = *it;
      view->onEntityAdded( entity );
   }
}

///////////////////////////////////////////////////////////////////////////////
