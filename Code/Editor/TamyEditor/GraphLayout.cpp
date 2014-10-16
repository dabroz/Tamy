#include "GraphLayout.h"


///////////////////////////////////////////////////////////////////////////////

GraphNodeCreationAction::GraphNodeCreationAction( GraphLayout& parent, const SerializableReflectionType& type, const QPointF& pos )
   : QAction( type.m_name.c_str(), &parent )
   , m_parent( parent )
   , m_type( type )
   , m_pos( pos )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void GraphNodeCreationAction::onTriggered()
{
   m_parent.add( m_type, m_pos );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GraphNodeRemoveAction::GraphNodeRemoveAction( GraphLayout& parent, const QList< GraphBlock* >& blocks )
   : QAction( "Remove nodes", &parent )
   , m_parent( parent )
   , m_blocks( blocks )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void GraphNodeRemoveAction::onTriggered()
{
   foreach( GraphBlock* block, m_blocks )
   {
      m_parent.remove( block );
   }
}

///////////////////////////////////////////////////////////////////////////////
