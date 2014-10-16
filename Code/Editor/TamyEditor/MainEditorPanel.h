/// @file   TamyEditor/MainEditorPanel.h
/// @brief  an interface for the main panel of the editor
#pragma once

#include <QtWidgets\QFrame>
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class QWidget;
class DockableTabWidget;
class QMainWindow;
class ResourceEditor;
class ClosableDockWidget;
class QSettings;

///////////////////////////////////////////////////////////////////////////////

struct TabLocation
{
   bool     m_internal;
   uint     m_index;

   TabLocation( bool internal = true, uint index = -1 ) : m_internal( internal ), m_index( index ) {}
};

///////////////////////////////////////////////////////////////////////////////

/**
 * An interface for the main panel of the editor.
 *
 * CAUTION: I could make particular widgets developed for this purpose implement
 * this interface directly, but  I want to keep the widgets as independent as possible - they're
 * just building blocks, that's all - so instead I'm creating proxy classes, such as SplittableTabsPanel,
 * that reference particular widgets.
 */
class MainEditorPanel : public QFrame
{
   Q_OBJECT

private:
   QMainWindow*               m_mainWindow;
   DockableTabWidget*         m_tabsWidget;

   std::vector< QWidget* >    m_undockedTabs;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param mainWindow
    */
   MainEditorPanel( QWidget* parentWidget, QMainWindow* mainWindow );
   ~MainEditorPanel();

   /**
    * Activates the tab at the specified location.
    *
    * @param location
    */
   void setActiveTab( const TabLocation& location );

   /**
    * Adds a new tab to the active tab widget, embedding the specified widget in it and 
    * labeling it with the specified label and icon.
    *
    * @param editor
    * @param icon
    * @param label
    * @param dock       should the editor be added to the editors tab manager ( Qt::NoDockWidgetArea ), or docked in a separate window
    */
   virtual void addEditor( ResourceEditor* editor, const QIcon& icon, QString& label, Qt::DockWidgetArea dockArea = Qt::NoDockWidgetArea );

   /**
    * Locates a tab in the tree with the specified name.
    *
    * @param tabName       name of the tab we're looking for
    * @param outLocation   location of the tab ( providing we found the tab of course )
    * @return              'true' if the tab was found, 'false' otherwise
    */
   bool findTabByName( const QString& tabName, TabLocation& outLocation ) const;

   /**
    * Assigns the specified tab a new name
    *
    * @param tabLocation
    * @param newName
    */
   void setTabName( const TabLocation& tabLocation, const QString& newName );

   /**
    * Collects all editors embedded in the tree.
    *
    * @param outEditors          editors will be appended to this array
    * @param outTabsLocations    if you want to learn their exact locations in the tree, you can specify a pointer to this array
    */
   void collectEditors( std::vector< ResourceEditor* >& outEditors, std::vector< TabLocation >* outTabsLocations = NULL ) const;

   /**
    * Removes the tab located under the specified location.
    *
    * @param location
    */
   void removeTab( const TabLocation& location );

   /**
    * Removes all tabs ( doesn't destroy the embedded widgets though ).
    */
   void clear();

   /**
    * Saves the layout info about all active ResourceEditors
    *
    * @param settings
    */
   void saveLayout( QSettings& outSettings );

   /**
    * Restores the layout of ResourceEditors.
    *
    * @param settings
    */
   void loadLayout( QSettings& outSettings );

public slots:
   void notifyTabClosed( QWidget* widget );
   void notifyTabUndocked( const QIcon& extractedPageIcon, const QString& extractedPageLabel, QWidget* extractedPageWidget );
   void notifyUndockedTabDestroyed( ClosableDockWidget* object );

signals:
   /**
    * A signal emitted when one of the tabs is closed.
    *
    * @param widget.
    */
   void onTabClosed( QWidget* widget );

private:
   void dockEditor( ResourceEditor* editor, const QIcon& icon, const QString& label, Qt::DockWidgetArea dockArea );
};

///////////////////////////////////////////////////////////////////////////////
