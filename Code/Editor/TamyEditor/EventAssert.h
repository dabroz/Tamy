/// @file   TamyEditor/EventAssert.h
/// @brief  event dispatched when the code hits an assertion
#pragma once

#include "core\Event.h"
#include "core\ThreadSafe.h"
#include "EventAssert.h"


///////////////////////////////////////////////////////////////////////////////

class AssertionHandle;

///////////////////////////////////////////////////////////////////////////////

class EventAssert : public Event
{
public:
   enum Choice
   {
      EAC_UNKNOWN = -1,
      EAC_BREAK = 0,
      EAC_IGNORE = 1,
      EAC_IGNORE_ALL = 2
   };

private:
   char*                                  m_whereabouts;
   char*                                  m_msg;
   TThreadSafe< EventAssert::Choice >&    m_choiceFlag;

public:
   /**
    * Constructor.
    *
    * @param msg
    * @param assertHandle
    */
   EventAssert( const char* whereabouts, const char* msg, TThreadSafe< EventAssert::Choice >& choiceFlag );
   ~EventAssert();

   // -------------------------------------------------------------------------
   // Event implementation
   // -------------------------------------------------------------------------
   void dispatch();
};

///////////////////////////////////////////////////////////////////////////////
