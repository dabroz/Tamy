#include "UISerializationUtil.h"
#include <QtWidgets\QWidget>
#include <QtCore\QSettings>
#include <QtWidgets\QDockWidget>
#include <QtWidgets\QTreeWidget>
#include <QtWidgets\QTabWidget>
#include <QtWidgets\QSplitter>
#include "MainEditorPanel.h"
#include "SerializableWidget.h"
#include "tamyeditor.h"
#include "TamySceneWidget.h"


///////////////////////////////////////////////////////////////////////////////

void UISerializationUtil::serialize( TamyEditor* rootWidget, MainEditorPanel* editorsPanel, bool save )
{
   // create the settings instance
   QSettings* uiSettings = new QSettings( "Coversion", "TamyEditor" );

   // serialize the editors manager
   {
      if ( save )
      {
         editorsPanel->saveLayout( *uiSettings );
      }
      else
      {
         editorsPanel->loadLayout( *uiSettings );
      }
   }

   // serialize the hierarchy of widgets
   {
      std::list< QWidget* >   widgetsQueue;
      widgetsQueue.push_back( rootWidget );

      while( !widgetsQueue.empty() )
      {
         QWidget* currWidget = widgetsQueue.front();
         widgetsQueue.pop_front();

         serializeWidgetSettings( *currWidget, *uiSettings, save );

         // analyze the widget's children
         const QObjectList& children = currWidget->children();
         foreach( QObject* obj, children )
         {
            QWidget* childWidget = dynamic_cast< QWidget* >( obj );
            if ( childWidget )
            {
               widgetsQueue.push_back( childWidget );
            }
         }
      }
   }

   // dispose of the settings instance
   delete uiSettings;
}

///////////////////////////////////////////////////////////////////////////////

void UISerializationUtil::serializeWidgetSettings( QWidget& widget, QSettings& outSettings, bool save )
{
   QString objName = widget.objectName();
   if ( objName.isEmpty() )
   {
      return;
   }
   outSettings.beginGroup( objName );

   // serialize common widget settings
   if ( save )
   {
      outSettings.setValue( "size", widget.size() );
      outSettings.setValue( "pos", widget.pos() );
   }
   else
   {
      widget.resize( outSettings.value( "size", widget.size() ).toSize() );
      widget.move( outSettings.value( "pos", widget.pos() ).toPoint() );
   }

   // serialize type specific settings (non-exclusive ifs block)
   {
      QDockWidget* dockWidget = dynamic_cast< QDockWidget* >( &widget );
      if ( dockWidget )
      {
         serializeDockWidgetSettings( *dockWidget, outSettings, save );
      }
   }

   {
      QTreeWidget* treeWidget = dynamic_cast< QTreeWidget* >( &widget );
      if ( treeWidget )
      {
         serializeTreeWidgetSettings( *treeWidget, outSettings, save );
      }
   }

   {
      QTabWidget* tabWidget = dynamic_cast< QTabWidget* >( &widget );
      if ( tabWidget )
      {
         serializeTabWidgetSettings( *tabWidget, outSettings, save );
      }
   }

   {
      SerializableWidget* serializableWidget = dynamic_cast< SerializableWidget* >( &widget );
      if ( serializableWidget )
      {
         if ( save )
         {
            serializableWidget->saveLayout( outSettings );
         }
         else
         {
            serializableWidget->loadLayout( outSettings );
         }
      }
   }

   {
      QSplitter* splitter = dynamic_cast< QSplitter* >( &widget );
      if ( splitter )
      {
         if ( save )
         {
            outSettings.setValue( "state", splitter->saveState() );
         }
         else
         {
             splitter->restoreState( outSettings.value( "state" ).toByteArray() );
         }
      }
   }

   {
      QMainWindow* mainWindow = dynamic_cast< QMainWindow* >( &widget );
      if ( mainWindow )
      {
         if ( save )
         {
            outSettings.setValue( "geometry", mainWindow->saveGeometry() );
            outSettings.setValue( "state", mainWindow->saveState() );
         }
         else
         {
            mainWindow->restoreGeometry( outSettings.value( "geometry" ).toByteArray() );
            mainWindow->restoreState( outSettings.value( "state" ).toByteArray() );
         }
      }
   }

   {
      TamySceneWidget* sceneRenderer = dynamic_cast< TamySceneWidget* >( &widget );
      if ( sceneRenderer )
      {
         if ( save )
         {
            sceneRenderer->saveUI( outSettings );
         }
         else
         {
            sceneRenderer->loadUI( outSettings );
         }
      }
   }

   // close the widget's settings group
   outSettings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////

void UISerializationUtil::serializeDockWidgetSettings( QDockWidget& widget, QSettings& outSettings, bool save )
{
   if ( save )
   {
      outSettings.setValue( "features", (int)widget.features() );
      outSettings.setValue( "floating", widget.isFloating() );
   }
   else
   {
      widget.setFeatures( ( QDockWidget::DockWidgetFeatures )outSettings.value( "features", (int)widget.features() ).toInt() );
      widget.setFloating( outSettings.value( "floating", widget.isFloating() ).toBool() );
   }

}

///////////////////////////////////////////////////////////////////////////////

void UISerializationUtil::serializeTreeWidgetSettings( QTreeWidget& widget, QSettings& outSettings, bool save )
{
   // gather info about present columns count
   QList< QVariant > columnSizes;
   int count = widget.columnCount();
   for ( int i = 0; i < count; ++i )
   {
      columnSizes.push_back( widget.columnWidth( i ) );
   }

   if ( save )
   {
      outSettings.setValue( "columnSizes", columnSizes );
   }
   else
   {
      columnSizes = outSettings.value( "columnSizes", columnSizes ).toList();

      // set the column sizes
      widget.setColumnCount( columnSizes.size() );
      int colIdx = 0;
      foreach( QVariant width, columnSizes )
      {
         widget.setColumnWidth( colIdx++, width.toInt() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void UISerializationUtil::serializeTabWidgetSettings( QTabWidget& widget, QSettings& outSettings, bool save )
{
   // serialize index of the active tab
   if ( save )
   {
      int activeTabIdx = widget.currentIndex();
      outSettings.setValue( "activeTabIdx", activeTabIdx );
   }
   else
   {
      int activeTabIdx = 0;
      activeTabIdx = outSettings.value( "activeTabIdx", activeTabIdx ).toInt();
      widget.setCurrentIndex( activeTabIdx );
   }
}

///////////////////////////////////////////////////////////////////////////////
