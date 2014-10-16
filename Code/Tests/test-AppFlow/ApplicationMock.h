#pragma once

#include "core-AppFlow\Application.h"
#include "core\Point.h"


///////////////////////////////////////////////////////////////////////////////

class ApplicationMock : public Application, public TimeDependent
{
private:
   bool m_initialized;
   bool m_hibernated;
   float m_timeElapsed;
   int m_receivedSignal;
   int m_sendSignalInsideStepLoop;

public:
   ApplicationMock(const std::string& name) 
         : Application(name), 
         m_initialized(false), 
         m_hibernated(false),
         m_timeElapsed(0),
         m_receivedSignal(-1),
         m_sendSignalInsideStepLoop(-1)
   {
      timeController().add( *this );
   }

   bool isInitialized() const  {return m_initialized; }

   bool isHibernated() const { return m_hibernated; }

   float getTimeElapsed() 
   {
      float result = m_timeElapsed;
      m_timeElapsed = 0;
      return result;
   }

   int getReceivedSignal() 
   {
      int result = m_receivedSignal;
      m_receivedSignal = -1;
      return result;
   }

   void sendSignal(const std::string& receiverApp, int signalId)
   {
      context().signal(*this, receiverApp, signalId);
   }

   void sendSignal(int signalId)
   {
      context().signal(*this, signalId);
   }

   void sendSignalFromStep(int signalId)
   {
      m_sendSignalInsideStepLoop = signalId;
   }

   void update(float timeElapsed)
   {
      m_timeElapsed = timeElapsed;

      if (m_sendSignalInsideStepLoop >= 0)
      {
         sendSignal(m_sendSignalInsideStepLoop);
         m_sendSignalInsideStepLoop = -1;
      }
   }

   void initialize()
   {
      m_initialized = true;
   }

   void deinitialize() 
   {
      m_initialized = false;
   }

   void hibernate()
   {
      m_hibernated = true;
   }

   void dehibernate()
   {
      m_hibernated = false;
   }

   void notify(const std::string& senderApp, int signalCode)
   {
      m_receivedSignal = signalCode;
   }
};

///////////////////////////////////////////////////////////////////////////////
