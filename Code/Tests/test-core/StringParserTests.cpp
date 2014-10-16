#include "core-TestFramework\TestFramework.h"
#include "core\StringParser.h"


///////////////////////////////////////////////////////////////////////////////

TEST( StringParser, simpleString )
{
   std::string testString = "uniform a uniform b < not something we're looking for > uniform cde";
   StringParser parser( &testString );

   std::string expectedWords [] = { "a", "b", "cde" };

   int iterationIdx = 0;
   std::string extractedWord;
   while ( parser.findNext( "uniform" ) )
   {
      CPPUNIT_ASSERT( parser.getNextWord( extractedWord  ) );
      CPPUNIT_ASSERT_EQUAL( expectedWords[iterationIdx], extractedWord );

      ++iterationIdx;
   }

   CPPUNIT_ASSERT_EQUAL( 3, iterationIdx );
}

///////////////////////////////////////////////////////////////////////////////

TEST( StringParser, noWordsJustTokens )
{
   std::string testString = "uniform ";
   StringParser parser( &testString );

   int iterationIdx = 0;
   std::string extractedWord;
   while ( parser.findNext( "uniform" ) )
   {
      CPPUNIT_ASSERT_EQUAL( false, parser.getNextWord( extractedWord ) );

      ++iterationIdx;
   }

   CPPUNIT_ASSERT_EQUAL( 1, iterationIdx );
}

///////////////////////////////////////////////////////////////////////////////

TEST( StringParser, extraDelimiters )
{
   std::string testString = "uniform a; uniform b;    uniform cde";
   StringParser parser( &testString );
   parser.addDelimiter( ";" );

   std::string expectedWords[] = { "a", "b", "cde" };

   int iterationIdx = 0;
   std::string extractedWord;
   while ( parser.findNext( "uniform" ) )
   {
      CPPUNIT_ASSERT( parser.getNextWord( extractedWord ) );
      CPPUNIT_ASSERT_EQUAL( expectedWords[iterationIdx], extractedWord );

      ++iterationIdx;
   }

   CPPUNIT_ASSERT_EQUAL( 3, iterationIdx );
}

///////////////////////////////////////////////////////////////////////////////

TEST( StringParser, parsingMultipleWordsInARow )
{
   std::string testString = "uniform a b c; uniform d e f; uniform gh ij kl";
   StringParser parser( &testString );
   parser.addDelimiter( ";" );

   std::string expectedWords [][3] = { { "a", "b", "c" }, { "d", "e", "f" }, { "gh", "ij", "kl" } };

   int iterationIdx = 0;
   std::string extractedWord[3];
   while ( parser.findNext( "uniform" ) )
   {
      for ( int i = 0; i < 3; ++i )
      {
         CPPUNIT_ASSERT( parser.getNextWord( extractedWord[i] ) );
         CPPUNIT_ASSERT_EQUAL( expectedWords[iterationIdx][i], extractedWord[i] );
      }

      ++iterationIdx;
   }

   CPPUNIT_ASSERT_EQUAL( 3, iterationIdx );
}

///////////////////////////////////////////////////////////////////////////////
