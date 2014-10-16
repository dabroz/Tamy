#include "StoryItemPropertyEditor.h"
#include "DropArea.h"
#include "StoryTreeItemMimeData.h"
#include "StoryBrowserTreeItem.h"
#include "tamyeditor.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include <QtWidgets\QPushButton>


///////////////////////////////////////////////////////////////////////////////

StoryItemPropertyEditor::StoryItemPropertyEditor( TEditableReflectionProperty< StoryItem* >* property )
   : QPropertyEditor( property->getLabel().c_str() )
   , m_property( property )
   , m_itemName( NULL )
   , m_droppedTreeItem( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryItemPropertyEditor::~StoryItemPropertyEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void StoryItemPropertyEditor::onInitialize()
{
   QWidget* frame = new QWidget( this );
   QHBoxLayout* layout = new QHBoxLayout( frame );
   layout->setSpacing(0);
   layout->setMargin(0);
   frame->setLayout( layout );
   addWidget( frame );

   // a field that shows the resource
   m_itemName = new DropArea( frame, new StoryTreeItemMimeData( m_droppedTreeItem ) );
   connect( m_itemName, SIGNAL( changed() ), this, SLOT( valChanged() ) );
   m_itemName->setMinimumSize( 20, 20 );
   m_itemName->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   layout->addWidget( m_itemName );

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

void StoryItemPropertyEditor::refreshPropertyName()
{
   StoryItem* item = m_property->get();
   if ( item )
   {
      m_itemName->setText( item->getName().c_str() );
   }
   else
   {
      m_itemName->setText( "<No item>");
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryItemPropertyEditor::valChanged()
{
   if ( m_droppedTreeItem )
   {
      StoryNode* node = m_droppedTreeItem->getNode();
      if ( node->isA< StoryItem >() )
      {
         StoryItem* droppedItem = static_cast< StoryItem* >( node );
         m_property->set( droppedItem );
         refreshPropertyName();
      }
   }

   m_droppedTreeItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryItemPropertyEditor::valErased( bool ) 
{
   m_property->set( NULL );
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void StoryItemPropertyEditor::onPropertyChanged()
{
   // do nothing
}

///////////////////////////////////////////////////////////////////////////////
