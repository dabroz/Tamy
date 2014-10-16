#include "EventAssert.h"
#include "tamyeditor.h"
#include "ToolsWindow.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

EventAssert::EventAssert( const char* whereabouts, const char* msg, TThreadSafe< EventAssert::Choice >& choiceFlag )
   : m_choiceFlag( choiceFlag )
{
   // copy the whereabouts string
   uint len = strlen( whereabouts );
   m_whereabouts = new char[len + 1];
   strncpy( m_whereabouts, whereabouts, len );
   m_whereabouts[len] = 0;

   // copy the message string
   len = strlen( msg );
   m_msg = new char[len + 1];
   strncpy( m_msg, msg, len );
   m_msg[len] = 0;
}

///////////////////////////////////////////////////////////////////////////////

EventAssert::~EventAssert()
{
   delete [] m_whereabouts;
   m_whereabouts = NULL;

   delete [] m_msg;
   m_msg = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void EventAssert::dispatch()
{
   TamyEditor& tamyEd = TamyEditor::getInstance();

   // deploy a new assertion dialog in a dedicated status box frame
   ToolsWindow* toolsWindow = tamyEd.getToolsWindow();
   toolsWindow->showAssert( m_whereabouts, m_msg, m_choiceFlag );
}

///////////////////////////////////////////////////////////////////////////////
