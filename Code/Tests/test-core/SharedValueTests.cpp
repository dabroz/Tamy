#include "core-TestFramework\TestFramework.h"
#include "core\SharedValue.h"


///////////////////////////////////////////////////////////////////////////////

TEST( SharedValue, basics )
{
   TShared< int > valueA = 5;
   TShared< int > valueB = valueA;

   CPPUNIT_ASSERT_EQUAL( 5, valueA.get() );
   CPPUNIT_ASSERT_EQUAL( 5, valueB.get() );

   valueB = 1;

   CPPUNIT_ASSERT_EQUAL( 1, valueA.get() );
   CPPUNIT_ASSERT_EQUAL( 1, valueB.get() );
}

///////////////////////////////////////////////////////////////////////////////
