#include "core.h"
#include "core/Profiler.h"
#include "core/Assert.h"
#include "core/Timer.h"


///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( Profiler );

///////////////////////////////////////////////////////////////////////////////

Profiler::Profiler( const SingletonConstruct& )
   : MAX_TRACE_LENGTH( 1024 )
   , m_tracesCount( 0 )
   , m_engineTimer( new CTimer() )
   , m_active( false ) 
{
   m_traces = new PTrace[ MAX_TRACE_LENGTH ];
   for ( uint i = 0; i < MAX_TRACE_LENGTH; ++i )
   {
      m_traces[i] = new Trace();
   }
}

///////////////////////////////////////////////////////////////////////////////

Profiler::Profiler( const Profiler& )
   : MAX_TRACE_LENGTH( 0 )
   , m_traces( NULL )
   , m_tracesCount( 0 )
   , m_engineTimer( NULL )
   , m_active( false ) 
{
   // restricted, shouldn't be called at all
   ASSERT( false );
}

///////////////////////////////////////////////////////////////////////////////

Profiler::~Profiler()
{
   delete m_engineTimer;
   m_engineTimer = NULL;

   for ( uint i = 0; i < MAX_TRACE_LENGTH; ++i )
   {
      delete m_traces[i];
   }
   delete [] m_traces;

   uint timersCount = m_timers.size();
   for ( uint i = 0; i < timersCount; ++i )
   {
      delete m_timers[i];
   }

   uint valueProfilersCount = m_valueProfilers.size();
   for ( uint i = 0; i < valueProfilersCount; ++i )
   {
      delete m_valueProfilers[i];
   }
}

///////////////////////////////////////////////////////////////////////////////

uint Profiler::registerTimer( const std::string& timerName )
{
   uint timerId = m_timers.size() + 1; // id = 0 is reserved
   m_timers.push_back( new Timer( timerName ) );
   return timerId;
}

///////////////////////////////////////////////////////////////////////////////

const std::string& Profiler::getTimerName( uint timerId ) const
{
   return m_timers[timerId - 1]->m_name;
}

///////////////////////////////////////////////////////////////////////////////

const std::string& Profiler::getValueProfilerName( uint valueProfilerId ) const
{
   return m_valueProfilers[valueProfilerId]->m_name;
}

///////////////////////////////////////////////////////////////////////////////

void Profiler::beginFrame()
{
   m_parentTimerIdx = 0;
   m_tracesCount = 0;
   m_stackTraceIdx = 0;
   m_active = true;

   uint timersCount = m_timers.size();
   for ( uint i = 0; i < timersCount; ++i )
   {
      Timer* timer = m_timers[i];
      timer->m_activeInstancesCount = 0;
      timer->m_activationTimestamp = -1;
      timer->m_timeElapsed = 0.0f;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Profiler::endFrame()
{
   m_active = false;
}

///////////////////////////////////////////////////////////////////////////////

void Profiler::start( uint timerId )
{
   if ( !m_active || m_tracesCount >= MAX_TRACE_LENGTH )
   {
      // no more room for traces
      return;
   }

   // stack trace management code
   {
      uint traceIdx = m_tracesCount;
      m_stackTraceIdx = traceIdx;
      ++m_tracesCount;

      // memorize the parent timer index
      m_traces[traceIdx]->m_parentTimerId = m_parentTimerIdx;
      m_parentTimerIdx = timerId;

      // memorize the id of a timer that left this trace
      m_traces[traceIdx]->m_timerId = timerId;
   }

   // timer activation code
   {
      Timer* timer = m_timers[timerId - 1];

      int& activeInstancesCount = timer->m_activeInstancesCount;
      if ( activeInstancesCount == 0 )
      {
         double& activationTimestamp = timer->m_activationTimestamp;

         // start counting only when the timer is activated for the first time.
         // If it's already been activated, and this is a nested activation, don't consider
         // this an activation and just keep on counting
         activationTimestamp = m_engineTimer->getCurrentTime();
      }

      // increase activations counter
      ++activeInstancesCount;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Profiler::end( uint timerId )
{
   if ( !m_active || m_tracesCount >= MAX_TRACE_LENGTH )
   {
      // no more room for traces
      return;
   }

   // measure the time as soon as you step into this method
   double currTime = m_engineTimer->getCurrentTime();

   // stack trace management code
   {
      uint traceIdx = m_stackTraceIdx; 
      --m_stackTraceIdx;

      // recover the parent timer index
      m_parentTimerIdx = m_traces[traceIdx]->m_parentTimerId;
   }

   // timer deactivation code
   {
      int& activeInstancesCount = m_timers[timerId - 1]->m_activeInstancesCount;

      // decrease activations counter
      ASSERT( activeInstancesCount > 0 );
      --activeInstancesCount;
      if ( activeInstancesCount == 0 )
      {
         // count the elapsed time only for the first activated instance
         Timer* timer = m_timers[timerId - 1];
         ASSERT( timer->m_activationTimestamp > 0 );

         timer->m_timeElapsed += ( currTime - timer->m_activationTimestamp );

         timer->m_activationTimestamp = -1;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

double Profiler::getProfiledValue( uint profilerId ) const
{
   ValueProfiler* profiler = m_valueProfilers[ profilerId ];
   double val = profiler->getValueAsDouble();

   return val;
}

///////////////////////////////////////////////////////////////////////////////

uint Profiler::getValueProfilersCount() const
{
   return m_valueProfilers.size();
}

///////////////////////////////////////////////////////////////////////////////
