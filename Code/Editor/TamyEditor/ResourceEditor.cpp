#include "ResourceEditor.h"
#include "core\ResourcesManager.h"
#include "core\CriticalSection.h"
#include <QTimer>
#include <QtWidgets\QLayout>
#include <QtWidgets\QWidget>


///////////////////////////////////////////////////////////////////////////////

ResourceEditor::ResourceEditor() 
   : QFrame( NULL ) 
   , m_enabled( true )
{
   m_updateTimer = new QTimer( this );
}

///////////////////////////////////////////////////////////////////////////////

ResourceEditor::~ResourceEditor() 
{
   delete m_updateTimer;
   m_updateTimer = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ResourceEditor::initialize( const QString& label, const QIcon& icon )
{
   m_label = label;
   m_icon = icon;
   m_enabled = true;

   m_updateTimer->start( 1 );

   onInitialize();
}

///////////////////////////////////////////////////////////////////////////////

void ResourceEditor::deinitialize( bool saveProgress ) 
{
   onDeinitialize( saveProgress );

   m_updateTimer->stop();
}


///////////////////////////////////////////////////////////////////////////////

void ResourceEditor::enable( bool flag )
{
   // gather all children and disable them
   QLayout* l = layout();
   uint widgetsCount = l->count();
   for ( int i = 0; i < widgetsCount; ++i )
   {
      QWidget* widget = l->itemAt(i)->widget();
      if ( widget )
      {
         widget->setEnabled( flag );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
