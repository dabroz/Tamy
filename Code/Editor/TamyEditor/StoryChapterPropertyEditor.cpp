#include "StoryChapterPropertyEditor.h"
#include "DropArea.h"
#include "StoryTreeItemMimeData.h"
#include "StoryBrowserTreeItem.h"
#include "tamyeditor.h"
#include "core\ResourcesManager.h"
#include "core\Filesystem.h"
#include <QtWidgets\QPushButton>


///////////////////////////////////////////////////////////////////////////////

StoryChapterPropertyEditor::StoryChapterPropertyEditor( TEditableReflectionProperty< StoryChapter* >* property )
   : QPropertyEditor( property->getLabel().c_str() )
   , m_property( property )
   , m_chapterName( NULL )
   , m_droppedTreeItem( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

StoryChapterPropertyEditor::~StoryChapterPropertyEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapterPropertyEditor::onInitialize()
{
   QWidget* frame = new QWidget( this );
   QHBoxLayout* layout = new QHBoxLayout( frame );
   layout->setSpacing(0);
   layout->setMargin(0);
   frame->setLayout( layout );
   addWidget( frame );

   // a field that shows the resource
   m_chapterName = new DropArea( frame, new StoryTreeItemMimeData( m_droppedTreeItem ) );
   connect( m_chapterName, SIGNAL( changed() ), this, SLOT( valChanged() ) );
   m_chapterName->setMinimumSize( 20, 20 );
   m_chapterName->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   layout->addWidget( m_chapterName );

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

void StoryChapterPropertyEditor::refreshPropertyName()
{
   StoryChapter* chapter = m_property->get();
   if ( chapter )
   {
      m_chapterName->setText( chapter->getName().c_str() );
   }
   else
   {
      m_chapterName->setText( "<No chapter>");
   }
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapterPropertyEditor::valChanged()
{
   if ( m_droppedTreeItem )
   {
      StoryNode* node = m_droppedTreeItem->getNode();
      if ( node->isA< StoryChapter >() )
      {
         StoryChapter* droppedChapter = static_cast< StoryChapter* >( node );
         m_property->set( droppedChapter );
         refreshPropertyName();
      }
   }

   m_droppedTreeItem = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapterPropertyEditor::valErased( bool ) 
{
   m_property->set( NULL );
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void StoryChapterPropertyEditor::onPropertyChanged()
{
   // do nothing
}

///////////////////////////////////////////////////////////////////////////////
