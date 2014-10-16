#include "core-TestFramework\TestFramework.h"
#include "core\Delegate.h"


///////////////////////////////////////////////////////////////////////////////

class SomeClass
{
private:
   int m_value;

public:
   SomeClass() : m_value(-1) {}

   void setValue(int val) {m_value = val;}

   int getValue() const {return m_value;}
};

///////////////////////////////////////////////////////////////////////////////

TEST(Delegate, callingSomeMethodViaDelegate)
{
   SomeClass classInstance;
   Delegate<void (int)> del = Delegate<void (int)>::from_method<SomeClass, &SomeClass::setValue> (&classInstance);

   del(20);
   CPPUNIT_ASSERT_EQUAL(20, classInstance.getValue());
};

///////////////////////////////////////////////////////////////////////////////
