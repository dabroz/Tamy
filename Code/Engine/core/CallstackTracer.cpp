#include "core.h"
#include "core\CallstackTracer.h"
#include <dbghelp.h>


///////////////////////////////////////////////////////////////////////////////

namespace 
{
   /**
    * Windows callstack tracer
    */
   class WinCallstackTracer
   {
   public:
      typedef BOOL    (__stdcall *tSymInitialize)( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
      typedef BOOL    (__stdcall *tSymRefreshModuleList)( IN HANDLE hProcess );
      typedef DWORD   (__stdcall *tSymGetOptions)( VOID );
      typedef DWORD   (__stdcall *tSymSetOptions)( IN DWORD SymOptions );
      typedef BOOL    (__stdcall *tSymFromAddr)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD64 pdwDisplacement, OUT PSYMBOL_INFO Symbol );
      typedef BOOL    (__stdcall *tStackWalk64)( DWORD MachineType, HANDLE hProcess, HANDLE hThread, LPSTACKFRAME64 StackFrame, PVOID ContextRecord, PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine, PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine, PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress );
      typedef PVOID   (__stdcall *tSymFunctionTableAccess64)( HANDLE hProcess, DWORD64 AddrBase );
      typedef DWORD64 (__stdcall *tSymGetModuleBase64)( IN HANDLE hProcess, IN DWORD64 dwAddr );
      typedef BOOL    (__stdcall *tSymGetLineFromAddr64)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_LINE64 Line );
      typedef BOOL    (__stdcall *tSymCleanup)( IN HANDLE hProcess );
      typedef void	 (__stdcall *tRtlCaptureContext)(OUT PCONTEXT context);    
      typedef USHORT  (__stdcall *tRtlCaptureStackBackTrace)(IN ULONG FramesToSkip, IN ULONG FramesToCapture, OUT PVOID *BackTrace, OUT PULONG BackTraceHash);

      tSymInitialize                      m_pSymInitialize;
      tSymRefreshModuleList               m_pSymRefreshModuleList;
      tSymGetOptions                      m_pSymGetOptions;
      tSymSetOptions                      m_pSymSetOptions;
      tSymFromAddr                        m_pSymFromAddr;
      tStackWalk64                        m_pStackWalk64;
      tSymFunctionTableAccess64           m_pSymFunctionTableAccess64;
      tSymGetModuleBase64                 m_pSymGetModuleBase64;
      tSymGetLineFromAddr64               m_pSymGetLineFromAddr64;
      tSymCleanup                         m_pSymCleanup;
      tRtlCaptureContext                  m_pRtlCaptureContext;
      tRtlCaptureStackBackTrace           m_pRtlCaptureStackBackTrace;

   private:
      static WinCallstackTracer           s_theInstance;

      CRITICAL_SECTION                    m_lock;
      bool                                m_lockInitialized;
      int                                 m_usageCount;
      HINSTANCE                           m_hDbgHelpDll;
      HINSTANCE                           m_hKernel32Dll;

   public:
      /**
       * Constructor.
       */
      WinCallstackTracer()
      {
         InitializeCriticalSection( &m_lock );
         m_lockInitialized = true;
      }

      ~WinCallstackTracer()
      {
         m_lockInitialized = false;
         DeleteCriticalSection( &m_lock );
      }

      /**
       * Returns the singleton instance of this class.
       */
      static WinCallstackTracer& getInstance() { return s_theInstance; }

      /**
       * Initializes the tracer implementation
       */
      void initialize()
      {
         enterLock();
         if( m_usageCount == 0 )
         {
            m_hDbgHelpDll = LoadLibrary( "dbghelp.dll" );
            ASSERT_MSG( m_hDbgHelpDll != NULL, "Unable to load dbghelp.dll");

            if ( m_hDbgHelpDll )
            {
               m_hKernel32Dll = LoadLibrary("kernel32.dll");
               ASSERT_MSG( m_hKernel32Dll != NULL, "Unable to load kernel32.dll");

               m_pSymInitialize              = (tSymInitialize)GetProcAddress( m_hDbgHelpDll, "SymInitialize" );
               m_pSymRefreshModuleList       = (tSymRefreshModuleList)GetProcAddress( m_hDbgHelpDll, "SymRefreshModuleList" );
               m_pSymGetOptions              = (tSymGetOptions)GetProcAddress( m_hDbgHelpDll, "SymGetOptions" );
               m_pSymSetOptions              = (tSymSetOptions)GetProcAddress( m_hDbgHelpDll, "SymSetOptions" );
               m_pSymFromAddr                = (tSymFromAddr)GetProcAddress( m_hDbgHelpDll, "SymFromAddr" );
               m_pStackWalk64                = (tStackWalk64)GetProcAddress( m_hDbgHelpDll, "StackWalk64" );
               m_pSymFunctionTableAccess64   = (tSymFunctionTableAccess64)GetProcAddress( m_hDbgHelpDll, "SymFunctionTableAccess64" );
               m_pSymGetModuleBase64         = (tSymGetModuleBase64)GetProcAddress( m_hDbgHelpDll, "SymGetModuleBase64" );
               m_pSymGetLineFromAddr64       = (tSymGetLineFromAddr64)GetProcAddress( m_hDbgHelpDll, "SymGetLineFromAddr64" );
               m_pSymCleanup                 = (tSymCleanup)GetProcAddress( m_hDbgHelpDll, "SymCleanup" );

               m_pRtlCaptureContext          = (tRtlCaptureContext)GetProcAddress( m_hKernel32Dll, "RtlCaptureContext" );
               m_pRtlCaptureStackBackTrace   = (tRtlCaptureStackBackTrace)GetProcAddress( m_hKernel32Dll, "RtlCaptureStackBackTrace" );

               DWORD symOptions = m_pSymGetOptions();
               symOptions |= SYMOPT_LOAD_LINES | SYMOPT_DEBUG | SYMOPT_DEFERRED_LOADS;
               m_pSymSetOptions( symOptions );

               // Temporarily suppress message box in case of error
#if (_WIN32_WINNT >= 0x0600)
               UINT prevErrMode = GetErrorMode();
               SetErrorMode(SEM_FAILCRITICALERRORS);
#endif

               // The stack tracer is used in a global static, so can't use error singleton
               m_pSymInitialize( GetCurrentProcess(), NULL, TRUE);

#if (_WIN32_WINNT >= 0x0600)
               SetErrorMode(prevErrMode);
#endif
            }
         }

         m_usageCount++;

         leaveLock();
      }

      /**
       * Deinitializes the implementation.
       */
      void deinitialize()
      {
         enterLock();
         if( --m_usageCount == 0 )
         {
            if( m_pSymCleanup )
            {
               m_pSymCleanup( GetCurrentProcess() );
            }
            m_pSymInitialize = 0;
            m_pSymRefreshModuleList = 0;
            m_pSymGetOptions = 0;
            m_pSymSetOptions = 0;
            m_pSymFromAddr = 0;
            m_pStackWalk64 = 0;
            m_pSymFunctionTableAccess64 = 0;
            m_pSymGetModuleBase64 = 0;
            m_pSymGetLineFromAddr64 = 0;
            m_pSymCleanup = 0;
            m_pRtlCaptureContext = 0;
            FreeLibrary( m_hKernel32Dll );
            FreeLibrary( m_hDbgHelpDll );
            m_hDbgHelpDll = 0;
         }

         leaveLock();
      }

   private:
      /**
       * Enters a critical section lock.
       */
      inline void enterLock()
      {
         if( m_lockInitialized )
         {
            EnterCriticalSection( &m_lock );
         }
      }

      /**
       * Leaves a critical section lock.
       */
      inline void leaveLock()
      {
         if( m_lockInitialized )
         {
            LeaveCriticalSection( &m_lock );
         }
      }
   };

   WinCallstackTracer WinCallstackTracer::s_theInstance;
}

///////////////////////////////////////////////////////////////////////////////

CallstackTracer::CallstackTracer()
{
   m_platformImpl = &WinCallstackTracer::getInstance();

   WinCallstackTracer* tracer = (WinCallstackTracer*)m_platformImpl;
   tracer->initialize();
}

///////////////////////////////////////////////////////////////////////////////

CallstackTracer::~CallstackTracer()
{
   WinCallstackTracer* tracer = (WinCallstackTracer*)m_platformImpl;
   tracer->deinitialize();
}

///////////////////////////////////////////////////////////////////////////////

uint CallstackTracer::getStackTrace( ulong* outArrTrace, uint traceMaxSize ) const
{
   WinCallstackTracer* tracer = (WinCallstackTracer*)m_platformImpl;

   if( tracer->m_pRtlCaptureStackBackTrace )
   {
      return tracer->m_pRtlCaptureStackBackTrace( 0, traceMaxSize, (PVOID*)outArrTrace, NULL );
   }

   return 0;
}

///////////////////////////////////////////////////////////////////////////////

void CallstackTracer::getTraceName( ulong traceId, char* outFileName, uint maxFileNameLength, char* outMethodName, uint maxMethodNameLength, uint& outLineNumber ) const
{
   memset( outFileName, 0, maxFileNameLength );
   memset( outMethodName, 0, maxMethodNameLength );
   outLineNumber = 0;

   WinCallstackTracer* tracer = (WinCallstackTracer*)m_platformImpl;
   if( !tracer->m_pSymGetLineFromAddr64 || !tracer->m_pSymFromAddr || maxMethodNameLength < 17 )
   {
      return;
   }

   SYMBOL_INFO_PACKAGE symbol;
   symbol.si.SizeOfStruct = sizeof( symbol.si );
   symbol.si.MaxNameLen = sizeof( symbol.name );

   HANDLE curproc = GetCurrentProcess();

   DWORD64 offset = 0;
   BOOL isSymbolCorrect = tracer->m_pSymFromAddr( curproc, DWORD64( traceId ), &offset, &symbol.si );
   if ( !isSymbolCorrect )
   {
      sprintf_s( outMethodName, maxMethodNameLength, "Incorrect symbol" );
      return;
   }

   IMAGEHLP_LINE64 line;
   memset( &line, 0, sizeof( line ) );
   line.SizeOfStruct = sizeof( line );

   tracer->m_pSymGetLineFromAddr64( curproc, DWORD64( traceId ), &offset, &line );

   if ( line.FileName )
   {
      strcpy_s( outFileName, maxFileNameLength, line.FileName );
   }

   if ( symbol.si.Name )
   {
      strcpy_s( outMethodName, maxMethodNameLength, symbol.si.Name );
   }

   outLineNumber = line.LineNumber;
}

///////////////////////////////////////////////////////////////////////////////

void CallstackTracer::printCallstack( std::ostream& outputStream, ulong* arrTrace, uint traceSize ) const
{
   char fileName[256];
   char methodName[256];
   uint lineNumber;
   std::string fullCallstack;
   char singleLine[640];

   outputStream << "\n";
   for ( uint i = 0; i < traceSize; ++i )
   {
      getTraceName( arrTrace[i], fileName, 256, methodName, 256, lineNumber );

      sprintf_s( singleLine, "%s ( %d ): %s\n", fileName, lineNumber, methodName );
      fullCallstack += singleLine;
   }
   fullCallstack += "\n===================================================================\n";
   outputStream << fullCallstack.c_str();
}

///////////////////////////////////////////////////////////////////////////////
