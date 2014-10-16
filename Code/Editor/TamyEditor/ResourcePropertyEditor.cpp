#include "ResourcePropertyEditor.h"
#include "DropArea.h"
#include "FSNodeMimeData.h"
#include "core.h"
#include "tamyeditor.h"
#include <QtWidgets\QToolButton>


///////////////////////////////////////////////////////////////////////////////

ResourcePropertyEditor::ResourcePropertyEditor( TEditableReflectionProperty< Resource* >* property )
: QPropertyEditor( property->getLabel().c_str() )
, m_property( property )
, m_resourceName( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

ResourcePropertyEditor::~ResourcePropertyEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void ResourcePropertyEditor::onInitialize()
{
   QWidget* frame = new QWidget( this );
   QHBoxLayout* layout = new QHBoxLayout( frame );
   layout->setSpacing(0);
   layout->setMargin(0);
   frame->setLayout( layout );
   addWidget( frame );

   // a field that shows the resource
   m_resourceName = new DropArea( frame, new FSNodeMimeData( m_paths ) );
   connect( m_resourceName, SIGNAL( changed() ), this, SLOT( valChanged() ) );
   m_resourceName->setMinimumSize( 20, 20 );
   m_resourceName->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   layout->addWidget( m_resourceName );

   // a button for NULLing the resource
   QToolButton* eraseButton = new QToolButton( frame );
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

void ResourcePropertyEditor::refreshPropertyName()
{
   Resource* res = m_property->get();
   if ( res )
   {
      m_resourceName->setText( res->getFilePath().c_str() );
   }
   else
   {
      m_resourceName->setText( "<No resource>");
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcePropertyEditor::valChanged()
{
   Resource* newVal = NULL;
   if ( !m_paths.empty() )
   {
      ResourcesManager& rm = TSingleton< ResourcesManager >::getInstance();
      newVal = rm.create( m_paths[ 0 ] );

      // verify that the types match
      const ReflectionType& newTypeClass = newVal->getVirtualRTTI();
      const ReflectionType& acceptableType = m_property->getType();
      if ( !newTypeClass.isA( acceptableType ) )
      {
         newVal = NULL;
      }
   }

   if ( newVal )
   {
      m_property->set( newVal );
      refreshPropertyName();
   }

   m_paths.clear();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcePropertyEditor::valErased( bool ) 
{
   m_property->set( NULL );
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcePropertyEditor::onPropertyChanged()
{
   refreshPropertyName();
}

///////////////////////////////////////////////////////////////////////////////
