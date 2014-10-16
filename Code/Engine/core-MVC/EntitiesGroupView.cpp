#include "core-MVC\EntitiesGroupView.h"
#include "core-MVC\EntitiesGroup.h"
#include "core\ListUtils.h"


///////////////////////////////////////////////////////////////////////////////

EntitiesGroupView::EntitiesGroupView()
{
}

///////////////////////////////////////////////////////////////////////////////

EntitiesGroupView::~EntitiesGroupView()
{
   while( m_groups.empty() == false )
   {
      EntitiesGroup* group = m_groups.front();
      m_groups.popFront();
      group->detachListener( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesGroupView::onAttachedToGroup( EntitiesGroup& group )
{
   List< EntitiesGroup* >::iterator it = ListUtils::find( m_groups, &group );
   if ( it.isEnd() )
   {
      m_groups.pushBack( &group );
   }
}

///////////////////////////////////////////////////////////////////////////////

void EntitiesGroupView::onDetachedFromGroup( EntitiesGroup& group )
{
   List< EntitiesGroup* >::iterator it = ListUtils::find( m_groups, &group );
   if ( !it.isEnd() )
   {
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////
