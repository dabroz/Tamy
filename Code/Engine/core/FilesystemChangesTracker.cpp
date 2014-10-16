#include "core\FilesystemChangesTracker.h"
#include "core\Filesystem.h"
#include "core\ThreadSystem.h"
#include "core\Thread.h"
#include "core\CriticalSection.h"
#include "core\Log.h"
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

FilesystemChangesTracker::FilesystemChangesTracker( Filesystem& fs )
   : m_fs( fs )
{
   m_runnable = new FilesystemChangesRunnable();

   m_thread = new Thread();
   m_thread->start( *m_runnable );
}

///////////////////////////////////////////////////////////////////////////////

FilesystemChangesTracker::~FilesystemChangesTracker()
{
   m_runnable->m_stop = true;
   m_thread->join();

   delete m_runnable;
   m_runnable = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemChangesTracker::restart()
{
   // let the runnable know we want to restart it
   m_runnable->m_restartPending = true;
   m_runnable->m_filesystemRoot = m_fs.getCurrRoot();
}

///////////////////////////////////////////////////////////////////////////////

bool FilesystemChangesTracker::processChanges()
{
   const bool changeFound = m_runnable->wasChangeDetected();
   return changeFound;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FilesystemChangesRunnable::FilesystemChangesRunnable()
   : m_stop( false )
   , m_restartPending( false )
   , m_initialized( false )
   , m_dwChangeHandle( 0 )
   , m_section( new CriticalSection() )
{
}

///////////////////////////////////////////////////////////////////////////////

FilesystemChangesRunnable::~FilesystemChangesRunnable()
{
   stopWatching();

   delete m_section;
   m_section = NULL;
}

///////////////////////////////////////////////////////////////////////////////

// @Main thread
bool FilesystemChangesRunnable::wasChangeDetected()
{
   m_section->enter();
   const bool changeFound = m_changeFound;
   m_changeFound = false;
   m_section->leave();

   return changeFound;
}

///////////////////////////////////////////////////////////////////////////////

// @Runner thread
void FilesystemChangesRunnable::run()
{
   while ( !m_stop )
   {
      if ( m_restartPending )
      {
         stopWatching();
         startWatching();
         m_restartPending = false;
      }

      if ( m_initialized )
      {
         processChanges();
      }

      // let the other threads do some processing
      Thread::yield();
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemChangesRunnable::startWatching()
{
   BOOL recursive = TRUE;

   // Watch the directory for file creation and deletion. 
   const DWORD flags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION;
   m_dwChangeHandle = FindFirstChangeNotification( m_filesystemRoot.c_str(), recursive, flags );
   if ( m_dwChangeHandle == INVALID_HANDLE_VALUE )
   {
      LOG( "FilesystemChangesTracker: Error (%d): FindFirstChangeNotification function failed.", GetLastError() );
      return;
   }

   // flag that we're ready to process
   m_initialized = true;
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemChangesRunnable::stopWatching()
{
   // flag that we're not ready to process
   m_initialized = false;

   if ( m_dwChangeHandle != 0 )
   {
      FindCloseChangeNotification( m_dwChangeHandle );
      m_dwChangeHandle = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemChangesRunnable::processChanges()
{
   DWORD dwWaitStatus = WaitForSingleObject( m_dwChangeHandle, 1000 );

   switch ( dwWaitStatus )
   {
      case WAIT_OBJECT_0:
      {
         m_section->enter();
         m_changeFound |= true;
         m_section->leave();

         if ( FindNextChangeNotification( m_dwChangeHandle ) == FALSE )
         {
            LOG( "FilesystemChangesTracker: Error (%d): FindNextChangeNotification function failed.", GetLastError() );
         }
         break;
      }

      case WAIT_TIMEOUT:
      {
         // restart the listener
         if ( FindNextChangeNotification( m_dwChangeHandle ) == FALSE )
         {
            LOG( "FilesystemChangesTracker: Error (%d): FindNextChangeNotification function failed.", GetLastError() );
         }
         break;
      }
   }

}

///////////////////////////////////////////////////////////////////////////////