#include "BlendTreeEventsListDelegate.h"
#include "BlendTreeEventEditor.h"
#include "BlendTreeEditorEventTrigger.h"
#include "core-AI\BlendTreeEvent.h"


///////////////////////////////////////////////////////////////////////////////

BTLEventsDelegate_Playing::BTLEventsDelegate_Playing( QObject *parent, BlendTreePlayer* blendTreePlayer )
   : QStyledItemDelegate( parent )
   , m_blendTreePlayer( blendTreePlayer )
{
}

///////////////////////////////////////////////////////////////////////////////

QWidget* BTLEventsDelegate_Playing::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   BlendTreeEditorEventTrigger* eventTrigger = new BlendTreeEditorEventTrigger( parent, m_blendTreePlayer );
   return eventTrigger;
}

///////////////////////////////////////////////////////////////////////////////

void BTLEventsDelegate_Playing::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
   BlendTreeEvent* btEvent = (BlendTreeEvent*)( index.model()->data( index, BTEL_EVENT_ROLE ).value< void* >() );

   BlendTreeEditorEventTrigger* eventTrigger = static_cast< BlendTreeEditorEventTrigger* >( editor );
   eventTrigger->initialize( btEvent );
}

///////////////////////////////////////////////////////////////////////////////

void BTLEventsDelegate_Playing::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void BTLEventsDelegate_Playing::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   editor->setGeometry( option.rect );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BTLEventsDelegate_Edition::BTLEventsDelegate_Edition( QObject *parent, BlendTree& blendTree )
   : m_blendTree( blendTree )
{
}

///////////////////////////////////////////////////////////////////////////////

QWidget* BTLEventsDelegate_Edition::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   BlendTreeEventEditor* eventEditor = new BlendTreeEventEditor( parent, m_blendTree );
   return eventEditor;
}

///////////////////////////////////////////////////////////////////////////////

void BTLEventsDelegate_Edition::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
   BlendTreeEvent* btEvent = (BlendTreeEvent*)( index.model()->data( index, BTEL_EVENT_ROLE ).value< void* >() );

   BlendTreeEventEditor* eventEditor = static_cast< BlendTreeEventEditor* >( editor );
   eventEditor->initialize( btEvent );
}

///////////////////////////////////////////////////////////////////////////////

void BTLEventsDelegate_Edition::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
   BlendTreeEventEditor* eventEditor = static_cast< BlendTreeEventEditor* >( editor );

   BlendTreeEvent* btEvent = (BlendTreeEvent*)( index.model()->data( index, BTEL_EVENT_ROLE ).value< void* >() );
   QString newEventName = eventEditor->getEventName();
   btEvent->setName( newEventName.toStdString().c_str() );

   model->setData( index, newEventName );
}

///////////////////////////////////////////////////////////////////////////////

void BTLEventsDelegate_Edition::updateEditorGeometry( QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   editor->setGeometry( option.rect );
}

///////////////////////////////////////////////////////////////////////////////
