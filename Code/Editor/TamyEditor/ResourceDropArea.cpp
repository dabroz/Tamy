#include "ResourceDropArea.h"
#include "DropArea.h"
#include "FSNodeMimeData.h"
#include <QtWidgets\QPushButton>
#include <QtWidgets\QHBoxLayout>
#include "core/ResourcesManager.h"


///////////////////////////////////////////////////////////////////////////////

ResourceDropArea::ResourceDropArea( QWidget* parentWidget, const FilePath& initialPath )
   : QFrame( parentWidget )
   , m_path( initialPath )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   layout->setSpacing(0);
   layout->setMargin(0);
   setLayout( layout );

   // a field that shows the resource
   m_resourceName = new DropArea( this, new FSNodeMimeData( m_paths ) );
   connect( m_resourceName, SIGNAL( changed() ), this, SLOT( valChanged() ) );
   m_resourceName->setMinimumSize( 20, 20 );
   m_resourceName->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   m_resourceName->setText( m_path.c_str() );
   layout->addWidget( m_resourceName );

   // a button for NULLing the resource
   QPushButton* eraseButton = new QPushButton( this );
   eraseButton->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/quit.png" ) );
   eraseButton->setToolTip( "Reset resource value" );
   connect( eraseButton, SIGNAL( clicked( bool ) ), this, SLOT( valErased( bool ) ) );
   eraseButton->setMinimumSize( 20, 20 );
   eraseButton->setMaximumSize( 20, 20 );
   eraseButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
   layout->addWidget( eraseButton );
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDropArea::setFilePath( const FilePath& path )
{
   m_paths.clear();
   if ( !path.empty() )
   {
      m_paths.push_back( path );
   }

   // disconnect the signal emitter - we don't want any signals emitted in response to this method's actions
   disconnect( m_resourceName, SIGNAL( changed() ), this, SLOT( valChanged() ) );

   refreshPropertyName();

   // reconnect the signal emitter
   connect( m_resourceName, SIGNAL( changed() ), this, SLOT( valChanged() ) );

}

///////////////////////////////////////////////////////////////////////////////

void ResourceDropArea::refreshPropertyName()
{
   if ( !m_paths.empty() )
   {
      m_path.set( m_paths[0] );
      m_resourceName->setText( m_paths[0].c_str() );
   }
   else
   {
      m_path.set( "" );
      m_resourceName->setText( "<No resource>");
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDropArea::valChanged()
{
   refreshPropertyName();
   m_paths.clear();

   emit pathChanged( m_path );
}

///////////////////////////////////////////////////////////////////////////////

void ResourceDropArea::valErased( bool ) 
{
   m_paths.clear();
   refreshPropertyName();

   emit pathChanged( m_path );
}

///////////////////////////////////////////////////////////////////////////////
