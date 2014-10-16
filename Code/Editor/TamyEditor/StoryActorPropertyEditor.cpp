#include "StoryActorPropertyEditor.h"
#include "DropArea.h"
#include "StoryTreeItemMimeData.h"
#include "StoryBrowserTreeItem.h"
#include "tamyeditor.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include <QtWidgets\QPushButton>


///////////////////////////////////////////////////////////////////////////////

StoryActorPropertyEditor::StoryActorPropertyEditor( TEditableReflectionProperty< StoryActor* >* property )
   : QPropertyEditor( property->getLabel().c_str() )
   , m_property( property )
   , m_actorName( NULL )
   , m_droppedTreeItem( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryActorPropertyEditor::~StoryActorPropertyEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void StoryActorPropertyEditor::onInitialize()
{
   QWidget* frame = new QWidget( this );
   QHBoxLayout* layout = new QHBoxLayout( frame );
   layout->setSpacing(0);
   layout->setMargin(0);
   frame->setLayout( layout );
   addWidget( frame );

   // a field that shows the resource
   m_actorName = new DropArea( frame, new StoryTreeItemMimeData( m_droppedTreeItem ) );
   connect( m_actorName, SIGNAL( changed() ), this, SLOT( valChanged() ) );
   m_actorName->setMinimumSize( 20, 20 );
   m_actorName->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   layout->addWidget( m_actorName );

   // a button for NULLing the resource
   QPushButton* eraseButton = new QPushButton( frame );
   eraseButton->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ) );
   eraseButton->setToolTip( "Reset resource value" );
   connect( eraseButton, SIGNAL( clicked( bool ) ), this, SLOT( valErased( bool ) ) );
   eraseButton->setMinimumSize( 20, 20 );
   eraseButton->setMaximumSize( 20, 20 );
   eraseButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
   layout->addWidget( eraseButton );

   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void StoryActorPropertyEditor::refreshPropertyName()
{
   StoryActor* actor = m_property->get();
   if ( actor )
   {
      m_actorName->setText( actor->getName().c_str() );
   }
   else
   {
      m_actorName->setText( "<No actor>");
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryActorPropertyEditor::valChanged()
{
   if ( m_droppedTreeItem )
   {
      StoryNode* node = m_droppedTreeItem->getNode();
      if ( node->isA< StoryActor >() )
      {
         StoryActor* droppedActor = static_cast< StoryActor* >( node );
         m_property->set( droppedActor );
         refreshPropertyName();
      }
   }

   m_droppedTreeItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryActorPropertyEditor::valErased( bool ) 
{
   m_property->set( NULL );
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void StoryActorPropertyEditor::onPropertyChanged()
{
   // do nothing
}

///////////////////////////////////////////////////////////////////////////////
