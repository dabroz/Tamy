#include "ToolsWindow.h"
#include "AssertFrame.h"
#include "ProfilerFrame.h"
#include "progressdialog.h"
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QLabel>


///////////////////////////////////////////////////////////////////////////////

ToolsWindow::ToolsWindow( QWidget* parentWidget )
   : ClosableDockWidget( "Tools", parentWidget )
   , m_activeProfilerView( NULL )
{
   m_mainTabs = new QTabWidget( this );
   setWidget( m_mainTabs );

   // add basic tabs
   m_asserts = new QTabWidget( m_mainTabs );
   m_asserts->setTabsClosable( true );
   connect( m_asserts, SIGNAL( tabCloseRequested( int ) ), this, SLOT( closeAssertTab( int ) ) );
   m_mainTabs->addTab( m_asserts, "Asserts" );

   QFrame* progressObserversFrame = new QFrame( m_mainTabs );
   m_progressObservers = new QVBoxLayout( progressObserversFrame );
   progressObserversFrame->setLayout( m_progressObservers );
   m_mainTabs->addTab( progressObserversFrame, "Operations" );

   m_messages = new QTextBrowser( m_mainTabs );
   m_mainTabs->addTab( m_messages, "Messages" );
}

///////////////////////////////////////////////////////////////////////////////

void ToolsWindow::showAssert( const char* whereabouts, const char* msg, TThreadSafe< EventAssert::Choice >& choiceFlag )
{
   AssertFrame* assertFrame = new AssertFrame( m_asserts, whereabouts, msg, choiceFlag );
   int tabIdx = m_asserts->addTab( assertFrame, whereabouts );

   // make the Asserts tab the active one
   m_mainTabs->setCurrentIndex( TOOLS_ASSERTIONS );
}

///////////////////////////////////////////////////////////////////////////////

void ToolsWindow::displayMessage( const char* label, const char* contents )
{
   QString displayedText;
   displayedText.sprintf( "%s\n-------------------------------------------------------------------------------\n\n%s\n", label, contents );

   m_messages->setText( displayedText );
   m_mainTabs->setCurrentIndex( TOOLS_MESSAGES );
}

///////////////////////////////////////////////////////////////////////////////

IProgressObserver& ToolsWindow::addProgressObserver( const char* label )
{
   ProgressObserverEntry* entry = new ProgressObserverEntry( m_progressObservers->parentWidget(), label );
   m_progressObservers->addWidget( entry );

   ProgressDialog* progressDialog = new ProgressDialog( entry );
   entry->setWidget( progressDialog );  

   // make the Operations tab the active one
   m_mainTabs->setCurrentIndex( TOOLS_OPERATIONS );

   return *progressDialog;
}

///////////////////////////////////////////////////////////////////////////////

void ToolsWindow::showProfilers()
{
   if ( m_activeProfilerView )
   {
      // find the profiler tab index and activate it

      int tabsCount = m_mainTabs->count();
      for ( int i = 0; i < tabsCount; ++i )
      {
         QString tabHeader = m_mainTabs->tabText( i );
         if ( tabHeader == "Profiler" )
         {
            m_mainTabs->setCurrentIndex( i );
            return;
         }
      }
   }

   m_activeProfilerView = new ProfilerFrame();
   connect( m_activeProfilerView, SIGNAL( onClosed() ), this, SLOT( profilerViewClosed() ) );
   int tabIdx = m_mainTabs->addTab( m_activeProfilerView, QString( "Profiler" ) );

   // activate the profiler tab
   m_mainTabs->setCurrentIndex( tabIdx );
}

///////////////////////////////////////////////////////////////////////////////

void ToolsWindow::profilerViewClosed()
{
   m_activeProfilerView = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ToolsWindow::closeAssertTab( int tabIdx )
{
   QWidget* embeddedWidget = m_asserts->widget( tabIdx );
   delete embeddedWidget;

}

///////////////////////////////////////////////////////////////////////////////

void ToolsWindow::update( float timeElapsed )
{
   // update profilers
   if ( m_activeProfilerView )
   {
      TSingleton< Profiler >::getInstance().endFrame();
      m_activeProfilerView->update( timeElapsed );
      TSingleton< Profiler >::getInstance().beginFrame();
   }

   // update progress dialogs
   {
      int widgetsCount = m_progressObservers->count();
      for ( int rowIdx = 0; rowIdx < widgetsCount; ++rowIdx )
      {
         QLayoutItem* widgetItem = m_progressObservers->itemAt( rowIdx );
         if ( !widgetItem )
         {
            continue;
         }
         ProgressObserverEntry* entry = static_cast< ProgressObserverEntry* >( widgetItem->widget() );
         ProgressDialog* progressDialog = static_cast< ProgressDialog* >( entry->getWidget() );

         bool hasFinished = progressDialog->update();
         if ( hasFinished )
         {
            m_progressObservers->removeItem( widgetItem );
            entry->deleteLater();
            delete widgetItem;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ProgressObserverEntry::ProgressObserverEntry( QWidget* parentWidget, const char* label )
   : QFrame( parentWidget )
   , m_embeddedWidget( NULL )
{
   QHBoxLayout* frameLayout = new QHBoxLayout( this );
   setLayout( frameLayout );

   frameLayout->addWidget( new QLabel( label, this ), 1 );
}

///////////////////////////////////////////////////////////////////////////////

ProgressObserverEntry::~ProgressObserverEntry()
{
}

///////////////////////////////////////////////////////////////////////////////

void ProgressObserverEntry::setWidget( QWidget* widget )
{
   m_embeddedWidget = widget;

   QHBoxLayout* frameLayout = static_cast< QHBoxLayout* >( layout() );
   frameLayout->addWidget( widget, 2 );
}

///////////////////////////////////////////////////////////////////////////////
