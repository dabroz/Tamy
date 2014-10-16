#include "DockableTabWidget.h"
#include <QtWidgets\QTabWidget>
#include <QtWidgets\QAction>
#include <QtWidgets\QMenu>
#include <QtWidgets\QMainWindow>
#include <QtWidgets\QDockWidget>
#include <QtWidgets\QHBoxLayout>


///////////////////////////////////////////////////////////////////////////////

DockableTabWidget::DockableTabWidget( QWidget* parentWidget, QMainWindow* mainWindow )
   : QTabWidget( parentWidget )
   , m_mainWindow( mainWindow )
{
   // add the tabs manager
   setContextMenuPolicy( Qt::CustomContextMenu );
   setTabsClosable( true );
   setMovable( true );
   connect( this, SIGNAL( tabCloseRequested ( int ) ), this, SLOT( closeTab( int ) ) );
   connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showPopupMenu( const QPoint& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void DockableTabWidget::showPopupMenu( const QPoint& pos )
{
   uint tabsCount = count();
   if ( tabsCount < 2 )
   {
      // there's no point in splitting the window if it's just got one tab in it
      return;
   }

   QPoint globalClickPos = mapToGlobal( pos );

   // show the pop-up only if you click a tab, and not the window contents
   {
      QWidget* w = widget(0);
      QPoint posInTab = w->mapFromGlobal( globalClickPos );
      if ( posInTab.y() >= 0 )
      {
         // it's already in the tab contents area - don't show the menu
         return;
      }
   }
   

   // create the menu
   QMenu* popupMenu = new QMenu( this );
   {
      QAction* undockAction = new QAction( "Undock", this );
      connect( undockAction, SIGNAL( triggered() ), this, SLOT( undockTab() ) );
      popupMenu->addAction( undockAction );
   }

   // display the menu
   popupMenu->popup( globalClickPos );
}

///////////////////////////////////////////////////////////////////////////////

void DockableTabWidget::undockTab()
{
   // extract currently active tab
   QIcon extractedPageIcon;
   QString extractedPageLabel;
   QWidget* extractedPageWidget = NULL;
   {
      int currentTabIdx = currentIndex();
      extractedPageIcon = tabIcon( currentTabIdx );
      extractedPageLabel = tabText( currentTabIdx );

      extractedPageWidget = currentWidget();
      removeTab( currentTabIdx );
   }

   // notify the observers
   emit onTabUndocked( extractedPageIcon, extractedPageLabel, extractedPageWidget );
}

///////////////////////////////////////////////////////////////////////////////

void DockableTabWidget::addAndActivateTab( QWidget* widget, const QIcon& icon, const QString& label )
{
   int addedTabIdx = addTab( widget, icon, label );
   setCurrentIndex( addedTabIdx );
}

///////////////////////////////////////////////////////////////////////////////

void DockableTabWidget::closeTab( int tabIdx )
{
   QWidget* closedTabWidget = widget( tabIdx );
   emit onTabClosed( closedTabWidget );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ClosableDockWidget::ClosableDockWidget( const QString& label, QWidget* parentWidget )
   : QDockWidget( label, parentWidget )
{
}

///////////////////////////////////////////////////////////////////////////////

void ClosableDockWidget::closeEvent( QCloseEvent* event )
{
   emit onClosed( this );
   __super::closeEvent( event );
}

///////////////////////////////////////////////////////////////////////////////
