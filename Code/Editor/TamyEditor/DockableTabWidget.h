/// @file   TamyEditor/DockableTabWidget.h
/// @brief  a tab manager widget with ability to extract separate tags and dock them at different positions
#pragma once

#include <QtWidgets\QTabWidget>
#include <QtWidgets\QDockWidget>


///////////////////////////////////////////////////////////////////////////////

class QMainWindow;
class ClosableDockWidget;

///////////////////////////////////////////////////////////////////////////////

/**
 * A tab manager widget with ability to extract separate tags and dock them at different positions.
 */
class DockableTabWidget : public QTabWidget
{
   Q_OBJECT

   QMainWindow*                     m_mainWindow;


public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param mainWindow
    */
   DockableTabWidget( QWidget* parentWidget, QMainWindow* mainWindow );

   /**
    * Adds a new tab and makes it immediately the active one.
    *
    * @param widget
    * @param icon
    * @param label
    */
   void addAndActivateTab( QWidget* widget, const QIcon& icon, const QString& label );

public slots:
   void showPopupMenu( const QPoint& pos );
   void undockTab();
   void closeTab( int tabIdx );

signals:
   /**
    * A signal emitted when one of the tabs is closed.
    *
    * @param widget.
    */
   void onTabClosed( QWidget* widget );

   /**
    * A signal emitted when one of the tabs is detached from the manager.
    *
    * @param extractedPageIcon
    * @param extractedPageLabel
    * @param extractedPageWidget
    */
   void onTabUndocked( const QIcon& extractedPageIcon, const QString& extractedPageLabel, QWidget* extractedPageWidget );
};

///////////////////////////////////////////////////////////////////////////////

class ClosableDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   /**
    * Constructor.
    *
    * @param label
    * @param parentWidget
    */
   ClosableDockWidget( const QString& label, QWidget* parentWidget );

signals:
   /**
    * A signal emitted when the widget is about to get closed.
    *
    * @param widget.
    */
   void onClosed( ClosableDockWidget* widget );

protected:
   virtual void closeEvent( QCloseEvent* event );
};

///////////////////////////////////////////////////////////////////////////////
