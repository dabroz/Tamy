#include "core-TestFramework\TestFramework.h"
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Algorithms, clamp )
{
   CPPUNIT_ASSERT_EQUAL( 0.5f, clamp< float >( 0.5f, 0.0f, 1.0f ) );
   CPPUNIT_ASSERT_EQUAL( 0.0f, clamp< float >( -0.5f, 0.0f, 1.0f ) );
   CPPUNIT_ASSERT_EQUAL( 0.2f, clamp< float >( 0.1f, 0.2f, 1.0f ) );
   CPPUNIT_ASSERT_EQUAL( 1.0f, clamp< float >( 1.1f, 0.2f, 1.0f ) );
}

///////////////////////////////////////////////////////////////////////////////

TEST(Misc, roundEx)
{
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0,  roundEx( 1.25, 1.0 ), 1e-3 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.2,  roundEx( 1.25, 0.1 ), 1e-3 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.25,  roundEx( 1.25, 0.01 ), 1e-3 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.25,  roundEx( 1.25, 0.001 ), 1e-3 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 125.0,  roundEx( 125.0, 1.0 ), 1e-3 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 120.0,  roundEx( 125.0, 10.0 ), 1e-3 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 100.0,  roundEx( 125.0, 100.0 ), 1e-3 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0,  roundEx( 125.0, 1000.0 ), 1e-3 );
}

///////////////////////////////////////////////////////////////////////////////

TEST(Misc, mag)
{
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f,  mag( 1.0 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f,  mag( 2.0 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 10.0f,  mag( 23.0 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 100.0f,  mag( 235.0 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 100.0f,  mag( 235.23 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.1f,  mag( 0.2 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.1f,  mag( 0.25 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.01f,  mag( 0.05 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.01f,  mag( 0.015 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.01f,  mag( 0.01 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.001f,  mag( 0.005 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.001f,  mag( 0.001 ), 1e-5 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0f,  mag( 0.0 ), 1e-5 );
}

///////////////////////////////////////////////////////////////////////////////
