#include "FilePathPropertyEditor.h"
#include "DropArea.h"
#include "FSNodeMimeData.h"
#include "core.h"
#include "tamyeditor.h"
#include <QtWidgets\QToolButton>


///////////////////////////////////////////////////////////////////////////////

FilePathPropertyEditor::FilePathPropertyEditor( TEditableReflectionProperty< FilePath >* property )
   : QPropertyEditor( property->getLabel().c_str() )
   , m_property( property )
   , m_droppedFilePath( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

FilePathPropertyEditor::~FilePathPropertyEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void FilePathPropertyEditor::onInitialize()
{
   QWidget* frame = new QWidget( this );
   QHBoxLayout* layout = new QHBoxLayout( frame );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   frame->setLayout( layout );
   addWidget( frame );

   // a field that shows the resource
   m_droppedFilePath = new DropArea( frame, new FSNodeMimeData( m_paths ) );
   connect( m_droppedFilePath, SIGNAL( changed() ), this, SLOT( valChanged() ) );
   m_droppedFilePath->setMinimumSize( 20, 20 );
   m_droppedFilePath->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   layout->addWidget( m_droppedFilePath );

   // a button for NULLing the resource
   QToolButton* eraseButton = new QToolButton( frame );
   eraseButton->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ) );
   eraseButton->setToolTip( "Reset value" );
   connect( eraseButton, SIGNAL( clicked( bool ) ), this, SLOT( valErased( bool ) ) );
   eraseButton->setMinimumSize( 20, 20 );
   eraseButton->setMaximumSize( 20, 20 );
   eraseButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
   layout->addWidget( eraseButton );

   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void FilePathPropertyEditor::refreshPropertyName()
{
   const FilePath& path = m_property->get();
   m_droppedFilePath->setText( path.c_str() );
}

///////////////////////////////////////////////////////////////////////////////

void FilePathPropertyEditor::valChanged()
{
   if ( !m_paths.empty() )
   {
      FilePath newVal = m_paths[0];
   
      m_property->set( newVal );
      refreshPropertyName();
   }

   m_paths.clear();
}

///////////////////////////////////////////////////////////////////////////////

void FilePathPropertyEditor::valErased( bool )
{
   m_property->set( FilePath() );
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void FilePathPropertyEditor::onPropertyChanged()
{
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////
