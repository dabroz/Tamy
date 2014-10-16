/// @file   core\TimeEvent.h
/// @brief  an event triggered from a time track
#pragma once

#include <string>
#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This interface represents a possibility of an action being executed 
 * at the specific point in time on a time track
 */
class TimeEvent
{
   DECLARE_ALLOCATOR( TimeEvent, AM_DEFAULT );

private:
   std::string m_id;

public:
   virtual ~TimeEvent() {}

   const std::string getID() const {return m_id;}

   virtual void execute() = 0;

protected:
   TimeEvent(const std::string& id) : m_id(id) {}
};

///////////////////////////////////////////////////////////////////////////////

template<typename T>
class TTimeEvent : public TimeEvent
{
   DECLARE_ALLOCATOR( TTimeEvent, AM_DEFAULT );

private:
   T& m_instance;

public:
   TTimeEvent(const std::string& id, T& instance) 
         : TimeEvent(id), m_instance(instance) 
   {}

   void execute()
   {
      m_instance.execute();
   }
};

///////////////////////////////////////////////////////////////////////////////
