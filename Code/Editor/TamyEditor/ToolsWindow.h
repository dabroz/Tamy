/// @file   TamyEditor/ToolsWindow.h
/// @brief  a window dedicated to displaying various editor tools
#pragma once

#include "DockableTabWidget.h"
#include "core\ThreadSafe.h"
#include "EventAssert.h"
#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class QTabWidget;
class QVBoxLayout;
class IProgressObserver;
class ProfilerFrame;
class QTextBrowser;

///////////////////////////////////////////////////////////////////////////////

enum ToolsWindowTabs
{
   TOOLS_ASSERTIONS,
   TOOLS_OPERATIONS,
   TOOLS_MESSAGES,
};

///////////////////////////////////////////////////////////////////////////////

class ToolsWindow : public ClosableDockWidget
{
   Q_OBJECT

private:
   QTabWidget*          m_mainTabs;

   QTabWidget*          m_asserts;
   QVBoxLayout*         m_progressObservers;

   ProfilerFrame*       m_activeProfilerView;

   QTextBrowser*        m_messages;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    */
   ToolsWindow( QWidget* parentWidget );

   /**
    * Displays an assert notification.
    *
    * @param whereabouts      description from where an assert was thrown
    * @param msg              assertion message
    * @param choiceFlag       user's decision how the assertion should be handled
    */
   void showAssert( const char* whereabouts, const char* msg, TThreadSafe< EventAssert::Choice >& choiceFlag );

   /**
    * Displays a custom message in a dedicated tab.
    *
    * @param label
    * @param contents
    */
   void displayMessage( const char* label, const char* contents );

   /**
    * Adds a new progress observer labeled with the specified label.
    *
    * @param label
    */
   IProgressObserver& addProgressObserver( const char* label );

   /**
    * Shows the profilers tab.
    */
   void showProfilers();

   /**
    * Updates state of particular widgets.
    *
    * @param timeElapsed
    */
   void update( float timeElapsed );

public slots:
   void profilerViewClosed();
   void closeAssertTab( int tabIdx );
};

///////////////////////////////////////////////////////////////////////////////

class ProgressObserverEntry : public QFrame
{
   Q_OBJECT

private:
   QWidget*       m_embeddedWidget;
public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param label
    */
   ProgressObserverEntry( QWidget* parentWidget, const char* label );
   ~ProgressObserverEntry();

   /**
    * Embeds a widget in the entry.
    *
    * @param widget
    */
   void setWidget( QWidget* widget );

   /**
    * Returns a pointer to the embedded widget.
    */
   inline QWidget* getWidget() const { return m_embeddedWidget; }
};

///////////////////////////////////////////////////////////////////////////////
