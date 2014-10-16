#include "core\CriticalSection.h"
#include "core\Assert.h"
#include "core\Thread.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

CriticalSection::CriticalSection()
{
   CRITICAL_SECTION* criticalSectionHandle = new CRITICAL_SECTION();
   InitializeCriticalSection( criticalSectionHandle );

   m_criticalSectionHandle = criticalSectionHandle;
}

///////////////////////////////////////////////////////////////////////////////

CriticalSection::~CriticalSection()
{
   if ( m_criticalSectionHandle )
   {
      CRITICAL_SECTION* criticalSectionHandle = (CRITICAL_SECTION*)m_criticalSectionHandle;
      DeleteCriticalSection( criticalSectionHandle );

      delete criticalSectionHandle;
      m_criticalSectionHandle = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

bool CriticalSection::tryEntering()
{
   ASSERT_MSG( m_criticalSectionHandle != NULL, "Critical section doesn't exist" );

   bool wasEntered = TryEnterCriticalSection( (CRITICAL_SECTION*)m_criticalSectionHandle );
   return wasEntered;
}

///////////////////////////////////////////////////////////////////////////////

void CriticalSection::enter()
{
   ASSERT_MSG( m_criticalSectionHandle != NULL, "Critical section doesn't exist" );

   EnterCriticalSection( (CRITICAL_SECTION*)m_criticalSectionHandle );
}

///////////////////////////////////////////////////////////////////////////////

void CriticalSection::leave()
{
   ASSERT_MSG( m_criticalSectionHandle != NULL, "Critical section doesn't exist" );

   LeaveCriticalSection( (CRITICAL_SECTION*)m_criticalSectionHandle );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CriticalSectionedSection::CriticalSectionedSection( CriticalSection& criticalSection, bool yieldAfterUnlock )
   : m_criticalSection( criticalSection )
   , m_yieldAfterUnlock( yieldAfterUnlock )
{
   m_criticalSection.enter();
}

///////////////////////////////////////////////////////////////////////////////

CriticalSectionedSection::~CriticalSectionedSection()
{
   m_criticalSection.leave();

   if ( m_yieldAfterUnlock )
   {
      Thread::yield();
   }
}

///////////////////////////////////////////////////////////////////////////////
