#include "SCTIComponent.h"
#include "core-MVC\Component.h"
#include <QtGui\QBrush>


///////////////////////////////////////////////////////////////////////////////

SCTIComponent::SCTIComponent( Component& component )
   : SceneTreeItem( false, false, false )
   , m_component( component )
{
   setForeground( 0, QBrush( QColor( 70, 70, 70 ) ) );
   updateDescription();
}

///////////////////////////////////////////////////////////////////////////////

SCTIComponent::~SCTIComponent()
{
}

///////////////////////////////////////////////////////////////////////////////

SceneNode* SCTIComponent::getNode() const
{
   return &m_component;
}

///////////////////////////////////////////////////////////////////////////////

bool SCTIComponent::canAccommodate( SceneNode* node ) const
{
   return false;
}

///////////////////////////////////////////////////////////////////////////////

void SCTIComponent::addNode( SceneNode* newNode )
{
   // not applicable
   ASSERT_MSG( false, "Components don't have any children" );
}

///////////////////////////////////////////////////////////////////////////////

void SCTIComponent::removeNode( SceneTreeItem* removedChildItem )
{
   // not applicable
   ASSERT_MSG( false, "Components don't have any children" );
}

///////////////////////////////////////////////////////////////////////////////

void SCTIComponent::clear()
{
   // not applicable
   ASSERT_MSG( false, "Components don't have any children" );
}

///////////////////////////////////////////////////////////////////////////////
