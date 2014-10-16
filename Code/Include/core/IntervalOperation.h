#pragma once


///////////////////////////////////////////////////////////////////////////////

template<typename Action>
class IntervalOperation
{
private:
   Action* m_action;
   float m_interval;
   float m_timeLine;
   float m_nextPointInTime;

public:
   IntervalOperation(float interval, Action* action) 
      : m_action(action), m_interval(interval), m_timeLine(0), m_nextPointInTime(interval)
   {}

   ~IntervalOperation()
   {
      delete m_action;
      m_action = NULL;
   }

   void operator()(float timeElapsed)
   {
      m_timeLine += timeElapsed;
      if (m_nextPointInTime >= m_timeLine)
      {
         (*m_action)();
         m_nextPointInTime += m_interval;
      }
   }
};

///////////////////////////////////////////////////////////////////////////////
