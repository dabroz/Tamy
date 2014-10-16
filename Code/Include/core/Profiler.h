/// @file   core/Profiler.h
/// @brief  profiling tool
#ifndef _PROFILER_H
#define _PROFILER_H

#include "core\MemoryRouter.h"
#include "core\types.h"
#include "core\Singleton.h"
#include <vector>
#include <string>


///////////////////////////////////////////////////////////////////////////////

class CTimer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Simple engine profiling tool.
 *
 * It's a singleton, so you can access it from any place you like.
 *
 * You can register a timer and then time a part of your code by embracing
 * it with `start` and `end` method calls.
 *
 * You can embed timing sections within other timing sections. The profiler
 * will keep track of those dependencies, but it will still track time for each of
 * the sections independently ( for accuracy and reliability reasons ).
 */
class Profiler
{
   PRIORITY_SINGLETON( 4 );
   DECLARE_ALLOCATOR( Profiler, AM_DEFAULT );

public:
   struct Timer
   {
      DECLARE_ALLOCATOR( Timer, AM_DEFAULT );

      std::string          m_name;
      double               m_activationTimestamp;
      double               m_timeElapsed;
      int                  m_activeInstancesCount;

      Timer( const std::string& name ) : m_name( name ), m_activationTimestamp( -1 ), m_timeElapsed( 0 ), m_activeInstancesCount( 0 ) {}
   };

   struct ValueProfiler
   {
      std::string          m_name;

      ValueProfiler( const std::string& name ) : m_name( name ) {}
      virtual ~ValueProfiler() {}

      virtual double getValueAsDouble() const = 0;
   };

   template< typename T >
   struct TValueProfiler : public ValueProfiler
   {
      DECLARE_ALLOCATOR( TValueProfiler, AM_DEFAULT );

      T                    m_value;

      TValueProfiler( const std::string& name ) : ValueProfiler( name ), m_value( 0  ) {}

      double getValueAsDouble() const { return (double)m_value; }
   };

   struct Trace 
   {
      DECLARE_ALLOCATOR( Trace, AM_DEFAULT );

      uint                 m_parentTimerId;
      uint                 m_timerId;
   };

private:
   const uint                       MAX_TRACE_LENGTH;

   CTimer*                          m_engineTimer;

   std::vector< Timer* >            m_timers;
   std::vector< ValueProfiler* >    m_valueProfilers;

   typedef Trace*                   PTrace;
   PTrace*                          m_traces;

   bool                             m_active;
   uint                             m_stackTraceIdx;
   uint                             m_tracesCount;

   uint                             m_parentTimerIdx;

public:
   /**
    * Singleton constructor.
    */
   Profiler( const SingletonConstruct&  );
   ~Profiler();

   // -------------------------------------------------------------------------
   // Timers management
   // -------------------------------------------------------------------------
   /**
    * Registers a new timer with the profiler.
    *
    * @param  timerName
    * @return timerId
    */
   uint registerTimer( const std::string& timerName );

   /**
    * Returns the name of the specified timer.
    *
    * @param timerId
    */
   const std::string& getTimerName( uint timerId ) const;

   /**
    * Activates the specified timer.
    *
    * @param timerId
    */
   void start( uint timerId );

   /**
    * Deactivates the specified timer, counting the time it was running
    * and storing the result for future queries.
    *
    * @param timerId
    */
   void end( uint timerId );

   /**
    * Returns the number of recorded traces.
    */
   inline uint getTracesCount() const { return m_tracesCount; }

   /**
    * Returns the specified trace details.
    *
    * @param traceIdx
    */
   inline const Trace& getTrace( uint traceIdx ) const { return *m_traces[traceIdx]; }


   /**
    * Returns the number of timers.
    */
   inline uint getTimersCount() const { return m_timers.size(); }

   /**
    * Returns the amount of time the timer was running this frame.
    *
    * @param timerId
    */
   inline double getTimeElapsed( uint timerId ) const { return m_timers[timerId - 1]->m_timeElapsed; }

   // -------------------------------------------------------------------------
   // Value profilers
   // -------------------------------------------------------------------------

   /**
    * Registers a new numerical value profiler.
    *
    * @param  timerName
    * @return profilerId
    */
   template< typename T >
   uint registerValueProfiler( const std::string& profilerName );

   /**
    * Returns the name of the specified value profiler.
    *
    * @param valueProfilerId
    */
   const std::string& getValueProfilerName( uint valueProfilerId ) const;

   /**
    * Updates the specified value profiler.
    *
    * @param profilerId
    */
   template< typename T >
   void updateValue( uint profilerId, T value );

   /**
    * Returns the profiled value as a double.
    *
    * @param profilerId
    */
   double getProfiledValue( uint profilerId ) const;

   /**
    * Returns a number of value profilers.
    */
   uint getValueProfilersCount() const;

   // -------------------------------------------------------------------------
   // Profiling frame API
   // -------------------------------------------------------------------------

   /**
    * Call this at the beginning of every engine frame to reset the measures from the previous frame.
    */
   void beginFrame();

   /**
    * Call this at the end of every frame to hand out the profiling results
    */
   void endFrame();

private:
   /**
    * Restricted copy constructor.
    */
   Profiler( const Profiler& );

   /**
    * This operation is restricted.
    */
   void operator=( const Profiler& profiler ) {}
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A helper class that profiles the scope it's been instantiated in.
 */
struct ScopedTimeProfiler
{
   uint        m_timerId;
   Profiler&   m_profiler;

   ScopedTimeProfiler( uint timerId )
      : m_timerId( timerId )
      , m_profiler( TSingleton< Profiler >::getInstance() )
   {
      m_profiler.start( m_timerId );
   }

   ~ScopedTimeProfiler()
   {
      m_profiler.end( m_timerId );
   }
};

///////////////////////////////////////////////////////////////////////////////
// Helper macros
///////////////////////////////////////////////////////////////////////////////

/**
 * Wrap a function declaration in this macro, and it will instantly start profiling
 * the entire method.
 */
#define PROFILED() \
   struct __FunctionTimeProfiler \
   { \
      uint m_timerId; \
      __FunctionTimeProfiler( const char* name ) \
      { \
         m_timerId = TSingleton< Profiler >::getInstance().registerTimer( name ); \
      } \
   }; \
   static __FunctionTimeProfiler profilerDecl( __FUNCTION__ ); \
   ScopedTimeProfiler __profiler( profilerDecl.m_timerId );


/**
 * Wrap a function declaration in this macro, and it will instantly start profiling
 * the entire method.
 */
#define PROFILE_VALUE( Type, Value ) \
   struct __FunctionValueProfiler \
   { \
      uint m_profilerId; \
      __FunctionValueProfiler( const char* name ) \
      { \
         m_profilerId = TSingleton< Profiler >::getInstance().registerValueProfiler< Type >( name ); \
      } \
   }; \
   static __FunctionValueProfiler profilerDecl( __FUNCTION__"::"#Value ); \
   TSingleton< Profiler >::getInstance().updateValue< Type >( profilerDecl.m_profilerId, Value );
   
///////////////////////////////////////////////////////////////////////////////

#include "core\Profiler.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _PROFILER_H
