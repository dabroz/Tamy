#pragma once

#include "core-Sound\SoundListener.h"


///////////////////////////////////////////////////////////////////////////////

class SoundListenerMock : public SoundListener
{
private:
   bool m_updated;

public:
   SoundListenerMock() 
         : m_updated(false)
   {}

   void update() {m_updated = true;}

   bool wasUpdated()
   {
      bool result = m_updated;
      m_updated = false;
      return result;
   }
};

///////////////////////////////////////////////////////////////////////////////
