#include "core-TestFramework\TestFramework.h"
#include "ext-2DGameLevel\GL2DLSystem.h"
#include "TypesRegistryInitializer.h"


///////////////////////////////////////////////////////////////////////////////

TEST( LSystem, simple )
{
   GL2DLSystem lSystem;
   lSystem.addRule( "a", "ab" );

   std::string output;
   lSystem.process( "a", 1, output );

   CPPUNIT_ASSERT_EQUAL( std::string( "ab" ), output );
}

///////////////////////////////////////////////////////////////////////////////

TEST( LSystem, twoRules )
{
   GL2DLSystem lSystem;
   lSystem.addRule( "a", "ab" );
   lSystem.addRule( "b", "a" );

   std::string output;
   lSystem.process( "a", 2, output );
   CPPUNIT_ASSERT_EQUAL( std::string( "aba" ), output );

   lSystem.process( "a", 3, output );
   CPPUNIT_ASSERT_EQUAL( std::string( "abaab" ), output );

   lSystem.process( "a", 4, output );
   CPPUNIT_ASSERT_EQUAL( std::string( "abaababa" ), output );

   lSystem.process( "a", 5, output );
   CPPUNIT_ASSERT_EQUAL( std::string( "abaababaabaab" ), output );

   lSystem.process( "a", 6, output );
   CPPUNIT_ASSERT_EQUAL( std::string( "abaababaabaababaababa" ), output );
}

///////////////////////////////////////////////////////////////////////////////
