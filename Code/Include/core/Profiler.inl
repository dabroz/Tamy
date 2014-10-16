#ifndef _PROFILER_H
#error "This file can only be included in Profiler.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< typename T >
uint Profiler::registerValueProfiler( const std::string& profilerName )
{
   uint profilerId = m_valueProfilers.size();
   m_valueProfilers.push_back( new TValueProfiler< T >( profilerName ) );
   return profilerId;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void Profiler::updateValue( uint profilerId, T value )
{
   TValueProfiler< T >* profiler = static_cast< TValueProfiler< T >* >( m_valueProfilers[ profilerId ] );

   profiler->m_value = value;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _PROFILER_H
