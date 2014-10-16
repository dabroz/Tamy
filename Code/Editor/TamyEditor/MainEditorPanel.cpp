#include "MainEditorPanel.h"
#include <QtWidgets\QHBoxLayout>
#include "DockableTabWidget.h"
#include "ResourceEditor.h"
#include <QtWidgets\QDockWidget>
#include <QtCore\QSettings>
#include <QtWidgets\QMainWindow>
#include "tamyeditor.h"
#include "core/Assert.h"
#include "core/ResourcesManager.h"
#include "Project.h"


///////////////////////////////////////////////////////////////////////////////

MainEditorPanel::MainEditorPanel( QWidget* parentWidget, QMainWindow* mainWindow )
   : QFrame( parentWidget )
   , m_mainWindow( mainWindow )
{
   setObjectName("MainEditorPanel");

   QHBoxLayout* layout = new QHBoxLayout();
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );
   
   // add the tabs manager
   m_tabsWidget = new DockableTabWidget( this, m_mainWindow );
   layout->addWidget( m_tabsWidget );
   connect( m_tabsWidget, SIGNAL( onTabClosed( QWidget* ) ), this, SLOT( notifyTabClosed( QWidget* ) ) );
   connect( m_tabsWidget, SIGNAL( onTabUndocked( const QIcon&, const QString&, QWidget* ) ), this, SLOT( notifyTabUndocked( const QIcon&, const QString&, QWidget* ) ) );
}

///////////////////////////////////////////////////////////////////////////////

MainEditorPanel::~MainEditorPanel()
{
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::setActiveTab( const TabLocation& location )
{
   if ( location.m_internal )
   {
      m_tabsWidget->setCurrentIndex( location.m_index );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::addEditor( ResourceEditor* editor, const QIcon& icon, QString& label, Qt::DockWidgetArea dockArea )
{
   if ( dockArea == Qt::NoDockWidgetArea )
   {
      m_tabsWidget->addAndActivateTab( editor, icon, label );
   }
   else
   {
      dockEditor( editor, icon, label, dockArea );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::dockEditor( ResourceEditor* editor, const QIcon& icon, const QString& label, Qt::DockWidgetArea dockArea )
{
   m_undockedTabs.push_back( editor );

   // create a new dockable tabs frame
   ClosableDockWidget* newDockWidget = new ClosableDockWidget( label, this );
   newDockWidget->setObjectName( label );
   newDockWidget->setWidget( editor );
   m_mainWindow->addDockWidget( dockArea, newDockWidget );

   connect( newDockWidget, SIGNAL( onClosed( ClosableDockWidget* ) ), this, SLOT( notifyUndockedTabDestroyed( ClosableDockWidget* ) ) );
}

///////////////////////////////////////////////////////////////////////////////

bool MainEditorPanel::findTabByName( const QString& tabName, TabLocation& outLocation ) const
{
   // first - go through the tabs in the tab manager
   uint tabsCount = m_tabsWidget->count();
   for ( uint i = 0; i < tabsCount; ++i )
   {
      QWidget* tabWidget = m_tabsWidget->widget( i );
      ASSERT_MSG( dynamic_cast< ResourceEditor* >( tabWidget ), "We don't support tabs that don't contain ResourceEditors" );

      ResourceEditor* editor = static_cast< ResourceEditor* >( tabWidget );
      if ( editor->getLabel() == tabName )
      {
         // found it
         outLocation.m_internal = true;
         outLocation.m_index = i;
         return true;
      }
   }

   // next - browse undocked editors
   uint undockedTabsCount = m_undockedTabs.size();
   for ( uint i = 0; i < undockedTabsCount; ++i )
   {
      ASSERT_MSG( dynamic_cast< ResourceEditor* >( m_undockedTabs[i] ), "We don't support tabs that don't contain ResourceEditors" );

      ResourceEditor* editor = static_cast< ResourceEditor* >( m_undockedTabs[i] );
      if ( editor->getLabel() == tabName )
      {
         // found it
         outLocation.m_internal = false;
         outLocation.m_index = i;
         return true;
      }
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::setTabName( const TabLocation& tabLocation, const QString& newName )
{
   if ( tabLocation.m_internal )
   {
      m_tabsWidget->setTabText( tabLocation.m_index, newName );
   }
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::collectEditors( std::vector< ResourceEditor* >& outEditors, std::vector< TabLocation >* outTabsLocations ) const
{
   // first - go through the tabs in the tab manager
   uint tabsCount = m_tabsWidget->count();
   for ( uint i = 0; i < tabsCount; ++i )
   {
      QWidget* tabWidget = m_tabsWidget->widget( i );

      ASSERT_MSG( dynamic_cast< ResourceEditor* >( tabWidget ), "We don't support tabs that don't contain ResourceEditors" );

      ResourceEditor* editor = static_cast< ResourceEditor* >( tabWidget );
      outEditors.push_back( editor );

      if ( outTabsLocations )
      {
         outTabsLocations->push_back( TabLocation( true, i ) );
      }
   }

   // next - browse undocked editors
   uint undockedTabsCount = m_undockedTabs.size();
   for ( uint i = 0; i < undockedTabsCount; ++i )
   {
      ASSERT_MSG( dynamic_cast< ResourceEditor* >( m_undockedTabs[i] ), "We don't support tabs that don't contain ResourceEditors" );

      ResourceEditor* editor = dynamic_cast< ResourceEditor* >( m_undockedTabs[i] );
      outEditors.push_back( editor );

      if ( outTabsLocations )
      {
         outTabsLocations->push_back( TabLocation( false, i ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::removeTab( const TabLocation& location )
{
   if ( location.m_internal )
   {
      m_tabsWidget->removeTab( location.m_index );
   }
   else
   {
      QWidget* parentWidget = m_undockedTabs[location.m_index]->parentWidget();
      QDockWidget* parentDocker = static_cast< QDockWidget* >( parentWidget );
      parentDocker->setParent( NULL );
      parentDocker->deleteLater();
   }
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::clear()
{
   m_tabsWidget->clear();
   m_undockedTabs.clear();
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::notifyTabClosed( QWidget* tabWidget )
{
   emit onTabClosed( tabWidget );
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::notifyTabUndocked( const QIcon& extractedPageIcon, const QString& extractedPageLabel, QWidget* extractedPageWidget )
{
   ASSERT_MSG( dynamic_cast< ResourceEditor* >( extractedPageWidget ), "We don't support tabs that don't contain ResourceEditors" );

   ResourceEditor* editor = static_cast< ResourceEditor* >( extractedPageWidget );
   dockEditor( editor, extractedPageIcon, extractedPageLabel, Qt::RightDockWidgetArea );
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::notifyUndockedTabDestroyed( ClosableDockWidget* dockWidget )
{
   QWidget* dockedWidget = dockWidget->widget();
   ASSERT_MSG( dynamic_cast< ResourceEditor* >( dockedWidget ), "We don't support tabs that don't contain ResourceEditors" );

   // remove the reference to this editor from our list
   uint undockedTabsCount = m_undockedTabs.size();
   for ( uint i = 0; i < undockedTabsCount; ++i )
   {
      if ( m_undockedTabs[i] == dockedWidget )
      {
         m_undockedTabs.erase( m_undockedTabs.begin() + i );
         break;
      }
   }

   // notify the observers
   emit onTabClosed( static_cast< ResourceEditor* >( dockedWidget )  );
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::saveLayout( QSettings& outSettings )
{
   char tmpEditorGroupStr[32];

   outSettings.beginGroup( "MainEditorPanel/activeEditors" );

   // first - serialize the active project's path, if there's one
   Project* activeProject = TamyEditor::getInstance().getActiveProject();
   if ( activeProject )
   {
      FilePath projectPath = activeProject->getFilePath();
      outSettings.setValue( "activeProjectPath", QString( projectPath.c_str() ) );
   }
   
   // if there's an active project, serialize the active editors ( editors may work only in the project scope )
   if ( activeProject )
   {
      std::vector< ResourceEditor* > editors;
      std::vector< TabLocation > tabsLocations;
      collectEditors( editors, &tabsLocations );

      uint count = editors.size();
      outSettings.setValue( "editorsCount", count );
      for ( uint i = 0; i < count; ++i )
      {
         ResourceEditor* editor = editors[i];

         // create a new group for each editor
         sprintf( tmpEditorGroupStr, "editor_%d", i );
         outSettings.beginGroup( tmpEditorGroupStr );
         {
            // save the layout data
            outSettings.setValue( "internal", tabsLocations[i].m_internal );
            if ( tabsLocations[i].m_internal == false )
            {
               QWidget* parentWidget = editor->parentWidget();
               ASSERT_MSG( dynamic_cast< QDockWidget* >( parentWidget ), "A detached ResourceEditor is expected to be embedded in a QDockWidget" );

               QDockWidget* parentDockWidget = static_cast< QDockWidget* >( parentWidget );
               Qt::DockWidgetArea dockArea = m_mainWindow->dockWidgetArea( parentDockWidget );
               outSettings.setValue( "dockArea", (int)dockArea );
            }

            outSettings.setValue( "path", editor->getLabel() );
            outSettings.setValue( "icon", editor->getIcon().name() );
         }
         outSettings.endGroup();
      }
   }
   outSettings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////

void MainEditorPanel::loadLayout( QSettings& outSettings )
{
   char tmpEditorGroupStr[32];
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
   TamyEditor& tamyEd = TamyEditor::getInstance();

   outSettings.beginGroup( "MainEditorPanel/activeEditors" );

   // first - deserialize the active project and load it
   Project* activeProject = NULL;
   {
      FilePath projectPath = outSettings.value( "activeProjectPath" ).toString().toStdString();
      activeProject = resMgr.create< Project >( projectPath, true );
      tamyEd.setActiveProject( activeProject );
   }

   // if the project was successfully restored, load the editors that were present 
   // when the layout was last saved with it
   if ( activeProject )
   {
      uint count = outSettings.value( "editorsCount" ).toInt();
      for ( uint i = 0; i < count; ++i )
      {
         // for each encountered editor group
         sprintf( tmpEditorGroupStr, "editor_%d", i );
         outSettings.beginGroup( tmpEditorGroupStr );
         {
            bool isInternal = outSettings.value( "internal" ).toBool();
            FilePath path( outSettings.value( "path" ).toString().toStdString() );

            Resource* resource = resMgr.create( path, true );
            if ( !resource )
            {
               // the resource doesn't exist any more - skip it
               outSettings.endGroup();
               continue;
            }

            // load the icon
            QString iconName = outSettings.value( "icon" ).toString();
            QIcon icon( iconName );

            ResourceEditor* editor = tamyEd.createResourceEditor( resource, icon );
            if ( !editor )
            {
               // for some reason we couldn't create an editor for this resource - so skip it
               outSettings.endGroup();
               continue;
            }

            Qt::DockWidgetArea dockArea = Qt::NoDockWidgetArea;
            if ( !isInternal )
            {
               // dock the resource editor
               dockArea = (Qt::DockWidgetArea)outSettings.value( "dockArea" ).toInt();
            }

            // add the editor tab
            QString resourcePath( path.c_str() );
            addEditor( editor, icon, resourcePath, dockArea );
         }
         outSettings.endGroup();
      }
   }
   outSettings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////
