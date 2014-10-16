#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>


///////////////////////////////////////////////////////////////////////////////

CppUnit::TestSuite& registerSuite(const char* name);

///////////////////////////////////////////////////////////////////////////////

template <class T>
class AutoRegisterFixture
{
  public:
    typedef void (T::*TestMethod)();
    AutoRegisterFixture(const char* suite, const char* name, TestMethod method)
    {
        CppUnit::TestSuite& s = registerSuite(suite);
        s.addTest(new CppUnit::TestCaller<T>(name, method));
    }
};

///////////////////////////////////////////////////////////////////////////////

#define TEST(suite, function)                                                 \
struct Test##suite##function : public CppUnit::TestFixture                    \
{                                                                             \
    void test##function();                                                    \
};                                                                            \
AutoRegisterFixture<Test##suite##function>                                    \
    autoRegister##suite##function(#suite, #suite"_"#function,                 \
        &Test##suite##function::test##function);                              \
void Test##suite##function::test##function()                                  

///////////////////////////////////////////////////////////////////////////////

#include "core-TestFramework\TestMacros.h"

///////////////////////////////////////////////////////////////////////////////
