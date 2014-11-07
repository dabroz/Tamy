#include "core-TestFramework\TestFramework.h"
#include "core\Log.h"

#include "core\MemoryRouter.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

namespace
{
   struct Noun
   {
      bool              m_male;
      std::string       m_subject;
   };

   struct Verb
   {
      std::string       m_present;
      std::string       m_past;
   };

   class Sentence
   {
   private:
      const Verb*          m_predicate;
      const Noun*          m_subject;
      const Noun*          m_complement;

      static const char*   s_pronouns[2];
      static const char*   s_possesivePronouns[2];

   public:
      Sentence& subject( const Noun& noun )
      {
         m_subject = &noun;
         return *this;
      }

      Sentence& predicate( const Verb& verb )
      {
         m_predicate = &verb;
         return *this;
      }

      Sentence& complement( const Noun& noun )
      {
         m_complement = &noun;
         return *this;
      }

      std::string toString() const
      {
         char tmpStr[256];
         sprintf_s( tmpStr, "%s %s %s", m_subject->m_subject.c_str(), m_predicate->m_past.c_str(), m_complement->m_subject.c_str() );
         return tmpStr;
      }

      std::string toAbbreviatedString() const
      {
         char tmpStr[256];
         sprintf_s( tmpStr, "%s %s %s", s_pronouns[m_subject->m_male], m_predicate->m_past.c_str(), s_possesivePronouns[m_complement->m_male] );
         return tmpStr;
      }
   };

   const char* Sentence::s_pronouns [2] = { "she", "he" };
   const char* Sentence::s_possesivePronouns [2] = { "her", "him" };


   class ComplexSentence
   {
   private:
      List< const Sentence* >    m_sentences;

   public:
      ComplexSentence& concat( const Sentence& sentence )
      {
         m_sentences.pushBack( &sentence );
         return *this;
      }

      std::string toString() const
      {
         std::string str;
         uint idx = 0;
         const uint count = m_sentences.size();
         for ( List< const Sentence* >::const_iterator it = m_sentences.begin(); !it.isEnd(); ++it, ++idx )
         {
            const Sentence* sentence = *it;
            if ( idx == 0 )
            {
               str = sentence->toString();
            }
            else
            {
               str += sentence->toAbbreviatedString();
            }

            if ( idx + 1 < count )
            {
               str += " and ";
            }
         }

         return str;
      }
   };
}
///////////////////////////////////////////////////////////////////////////////

TEST( Sentence, complexSentence )
{
   Noun jerry;
   jerry.m_male = true;
   jerry.m_subject = "Jerry";

   Noun rhonda;
   rhonda.m_male = false;
   rhonda.m_subject = "Rhonda";

   Noun dataServer;
   dataServer.m_male = false;
   dataServer.m_subject = "data server";

   Verb ask;
   ask.m_present = "ask";
   ask.m_past = "asked";

   Verb remember;
   remember.m_present = "remember";
   remember.m_past = "remembered";

   {
      Sentence sentence1;
      Sentence sentence2;
      ComplexSentence complexSentence;
      complexSentence.concat( sentence1 ).concat( sentence2 );
      sentence1.subject( jerry ).predicate( ask ).complement( rhonda );
      sentence2.subject( rhonda ).predicate( remember ).complement( jerry );

      CPPUNIT_ASSERT_EQUAL( std::string( "Jerry asked Rhonda and she remembered him" ), complexSentence.toString() );
   }

   {
      Sentence sentence1;
      Sentence sentence2;
      ComplexSentence complexSentence;
      complexSentence.concat( sentence1 ).concat( sentence2 );
      sentence1.subject( rhonda ).predicate( ask ).complement( jerry );
      sentence2.subject( jerry ).predicate( remember ).complement( rhonda );

      CPPUNIT_ASSERT_EQUAL( std::string( "Rhonda asked Jerry and he remembered her" ), complexSentence.toString() );
   }

   {
      Sentence sentence1;
      Sentence sentence2;
      ComplexSentence complexSentence;
      complexSentence.concat( sentence1 ).concat( sentence2 );
      sentence1.subject( rhonda ).predicate( ask ).complement( jerry );
      sentence2.subject( jerry ).predicate( remember ).complement( dataServer );

      CPPUNIT_ASSERT_EQUAL( std::string( "Rhonda asked Jerry and he remembered the data server" ), complexSentence.toString() );
   }

   {
      Sentence sentence1;
      Sentence sentence2;
      ComplexSentence complexSentence;
      complexSentence.concat( sentence1 ).concat( sentence2 );
      sentence1.subject( rhonda ).predicate( ask ).complement( dataServer );
      sentence2.subject( dataServer ).predicate( remember ).complement( jerry );

      CPPUNIT_ASSERT_EQUAL( std::string( "Rhonda asked the data server and it remembered Jerry" ), complexSentence.toString() );
   }
}

///////////////////////////////////////////////////////////////////////////////
