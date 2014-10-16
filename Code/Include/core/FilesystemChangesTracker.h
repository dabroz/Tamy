/// @file   core\FilesystemChangesTracker.h
/// @brief  a filesystem scanner used internally by the Filesystem to scan for changes in the files structure
#pragma once

#include "core\Runnable.h"
#include "core\MemoryRouter.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class Filesystem;
class Thread;
class CriticalSection;

///////////////////////////////////////////////////////////////////////////////

class FilesystemChangesTracker
{
   DECLARE_ALLOCATOR( FilesystemChangesTracker, AM_DEFAULT );

private:
   Filesystem&                            m_fs;
   Thread*                                m_thread;
   class FilesystemChangesRunnable*       m_runnable;

public:
   /**
    * Constructor.
    *
    * @param fs
    */
   FilesystemChangesTracker( Filesystem& fs );
   ~FilesystemChangesTracker();

   /**
    * Restarts the scanner with new filesystem settings.
    */
   void restart();

   /**
    * Processes filesystem changes.
    *
    * @return 'true' if changes were found, 'false' otherwise
    */
   bool processChanges();

   // -------------------------------------------------------------------------
   // Runnable implementation
   // -------------------------------------------------------------------------
   void run();
};

///////////////////////////////////////////////////////////////////////////////

class FilesystemChangesRunnable : public Runnable
{
   DECLARE_ALLOCATOR( FilesystemChangesRunnable, AM_DEFAULT );

public:
   bool                 m_stop;     // a flag that is set when the controller wants to stop the thread
                              // this runnable is running on

   bool                 m_restartPending; // a flag set when the runnable needs to restart, because the filesystem root changed
   std::string          m_filesystemRoot;

   bool                 m_initialized;
   void*                m_dwChangeHandle;

   bool                 m_changeFound;
   CriticalSection*     m_section;

public:
   /**
    * Constructor.
    */
   FilesystemChangesRunnable();
   ~FilesystemChangesRunnable();

   /**
    * Tells if a change was detected.
    *
    * @called from the main thread
    */
   bool wasChangeDetected();

   // -------------------------------------------------------------------------
   // Runnable implementation
   // -------------------------------------------------------------------------
   void run();

private:
   void startWatching();
   void stopWatching();
   void processChanges();
};

///////////////////////////////////////////////////////////////////////////////
