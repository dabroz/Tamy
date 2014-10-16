#include "BlendTreeEventEditor.h"
#include "core-AI\BlendTree.h"
#include "core-AI\BlendTreeEvent.h"

// ui elements
#include <QtWidgets\QLineEdit>
#include <QtWidgets\QPushButton>
#include <QtWidgets\QHBoxLayout>


///////////////////////////////////////////////////////////////////////////////

BlendTreeEventEditor::BlendTreeEventEditor( QWidget* parent, BlendTree& blendTree )
   : QFrame( parent )
   , m_blendTree( blendTree )
   , m_event( NULL )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   m_eventName = new QLineEdit( this );
   layout->addWidget( m_eventName );

   QPushButton* removeEventButton = new QPushButton( this );
   removeEventButton->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ) );
   connect( removeEventButton, SIGNAL( clicked( bool ) ), this, SLOT( removeEvent( bool ) ) );
   layout->addWidget( removeEventButton );
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeEventEditor::~BlendTreeEventEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEventEditor::initialize( BlendTreeEvent* event )
{
   m_event = event;
   m_eventName->setText( m_event->getName().c_str() );
}

///////////////////////////////////////////////////////////////////////////////

QString BlendTreeEventEditor::getEventName() const
{
   return m_eventName->text();
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeEventEditor::removeEvent( bool )
{
   m_blendTree.removeEvent( m_event );
}

///////////////////////////////////////////////////////////////////////////////
