#include "core.h"
#include "core/Assert.h"
#include <cassert>
#include <stdio.h>
#include <stdarg.h>


///////////////////////////////////////////////////////////////////////////////

int AssertionHandle::s_handle = 0;

AssertionHandle::AssertionHandle( const char* function, const char* file, unsigned int line ) 
   : m_function( function )
   , m_file( file )
   , m_line( line )
   , m_ignored( false )
   , m_handle( s_handle++ )
{ 
}

///////////////////////////////////////////////////////////////////////////////

TamyAssertFunc _tamyAssert;

///////////////////////////////////////////////////////////////////////////////

class DefaultAssertFunc
{
private:
   static DefaultAssertFunc s_registrationMechanism;

public:
     DefaultAssertFunc()
     {
        _tamyAssert = &myAssert;
     }

     static void myAssert( const char* msg, AssertionHandle& assertHandle )
     {
        assert( msg );
     }
};
DefaultAssertFunc DefaultAssertFunc::s_registrationMechanism;

///////////////////////////////////////////////////////////////////////////////

void registerAssertCallback( TamyAssertFunc func )
{
   _tamyAssert = func;
}

///////////////////////////////////////////////////////////////////////////////
