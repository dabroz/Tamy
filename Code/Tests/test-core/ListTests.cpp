#include "core-TestFramework\TestFramework.h"
#include "core\List.h"
#include "core\ListUtils.h"
#include "core\ContinuousMemoryPool.h"


///////////////////////////////////////////////////////////////////////////////

TEST(List, regularOperations)
{
   List< int > l;

   l.pushBack(1);
   CPPUNIT_ASSERT_EQUAL( 1, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );

   l.pushBack(2);
   CPPUNIT_ASSERT_EQUAL( 2, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );

   l.pushBack(3);
   CPPUNIT_ASSERT_EQUAL( 3, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );

   l.pushFront(0);
   CPPUNIT_ASSERT_EQUAL( 3, l.back() );
   CPPUNIT_ASSERT_EQUAL( 0, l.front() );

   l.popFront();
   CPPUNIT_ASSERT_EQUAL( 3, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );

   l.popBack();
   CPPUNIT_ASSERT_EQUAL( 2, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );
}

///////////////////////////////////////////////////////////////////////////////

TEST(List, customAllocator)
{
   ContinuousMemoryPool allocator( 256 );
   List< int > l( &allocator );

   l.pushBack(1);
   CPPUNIT_ASSERT_EQUAL( 1, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );

   l.pushBack(2);
   CPPUNIT_ASSERT_EQUAL( 2, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );

   l.pushBack(3);
   CPPUNIT_ASSERT_EQUAL( 3, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );

   l.pushFront(0);
   CPPUNIT_ASSERT_EQUAL( 3, l.back() );
   CPPUNIT_ASSERT_EQUAL( 0, l.front() );

   l.popFront();
   CPPUNIT_ASSERT_EQUAL( 3, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );

   l.popBack();
   CPPUNIT_ASSERT_EQUAL( 2, l.back() );
   CPPUNIT_ASSERT_EQUAL( 1, l.front() );
}

///////////////////////////////////////////////////////////////////////////////

TEST(List, removingSelectedElements)
{
    List< int > l;
    for ( int i = 0; i < 4; ++i )
    {
      l.pushBack( i );
    }

    // remove a middle element
    {
       int i = 0;
       for ( List< int >::iterator it = l.begin(); !it.isEnd(); ++it, ++i )
       {
          if ( i == 1 )
          {
             l.remove( it );
             break;
          }
       }

       CPPUNIT_ASSERT_EQUAL( (uint)3, l.size() );

       int expectedElems[] = { 0, 2, 3 };
       i = 0;
       for ( List< int >::iterator it = l.begin(); !it.isEnd(); ++it, ++i )
       {
          CPPUNIT_ASSERT_EQUAL( expectedElems[i], *it );
       }
    }


    // remove the first elem
    {
      l.remove( l.begin() );
      CPPUNIT_ASSERT_EQUAL( (uint)2, l.size() );

      int expectedElems[] = { 2, 3 };
      int i = 0;
      for ( List< int >::iterator it = l.begin(); !it.isEnd(); ++it, ++i )
      {
         CPPUNIT_ASSERT_EQUAL( expectedElems[i], *it );
      }
    }

    // remove the last elem
    {
       l.remove( l.end() );
       CPPUNIT_ASSERT_EQUAL( (uint)1, l.size() );

       int expectedElems[] = { 2 };
       int i = 0;
       for ( List< int >::iterator it = l.begin(); !it.isEnd(); ++it, ++i )
       {
          CPPUNIT_ASSERT_EQUAL( expectedElems[i], *it );
       }
    }

    // remove the remaining element and make sure the queue is empty
    {
       l.remove( l.begin() );
       CPPUNIT_ASSERT( l.empty() );
       CPPUNIT_ASSERT_EQUAL( (uint)0, l.size() );
    }
}

///////////////////////////////////////////////////////////////////////////////

TEST( List, removingItemsDuringIteration )
{
   // create a list
   List< int > l;

   // removing head elements
   {
      for ( int i = 0; i < 4; ++i )
      {
         l.pushBack( i );
      }


      for ( List< int >::iterator it = l.begin(); !it.isEnd(); ++it )
      {
         it.markForRemoval();
      }

      CPPUNIT_ASSERT( l.empty() );
   }

   // removing tail elements
   {
      for ( int i = 0; i < 4; ++i )
      {
         l.pushBack( i );
      }


      for ( List< int >::iterator it = l.end(); !it.isEnd(); --it )
      {
         it.markForRemoval();
      }

      CPPUNIT_ASSERT( l.empty() );
   }

   // removing middle elements
   {
      for ( int i = 0; i < 4; ++i )
      {
         l.pushBack( i );
      }


      {
         List< int >::iterator it = l.begin();
         ++it;
         for ( ; !it.isEnd(); ++it )
         {
            it.markForRemoval();
         }
      }

      CPPUNIT_ASSERT_EQUAL( (uint)1, l.size() );
      CPPUNIT_ASSERT_EQUAL( 0, l.front() );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( ListUtils, insertSorted )
{
   List< int > l;

   ListUtils::insertSorted( l, 2 );
   ListUtils::insertSorted( l, 1 );
   ListUtils::insertSorted( l, 5 );
   ListUtils::insertSorted( l, 4 );
   ListUtils::insertSorted( l, 3 );
   ListUtils::insertSorted( l, 0 );

   CPPUNIT_ASSERT_EQUAL( (uint)6, l.size() );

   int expectedVal = 0;
   for ( List< int >::iterator it = l.begin(); !it.isEnd(); ++it, ++expectedVal )
   {
      CPPUNIT_ASSERT_EQUAL( expectedVal, *it );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST(List, copyingLists)
{
   List< int > original;
   for ( int i = 0; i < 10; ++i )
   {
      original.pushBack( i );
   }

   List<int> copy( original );
   
   // compare lists
   CPPUNIT_ASSERT_EQUAL( copy.size(), original.size() );
   List< int >::iterator origIt = original.begin();
   List< int >::iterator copyIt = copy.begin();
   for ( ; !origIt.isEnd(); ++origIt, ++copyIt )
   {
      CPPUNIT_ASSERT_EQUAL( *copyIt, *origIt );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( ListUtils, checkingDifferences )
{
   List< int > oldList;
   List< int > newList;

   oldList.pushBack( 5 );
   oldList.pushBack( 1 );
   oldList.pushBack( 6 );

   newList.pushBack( 1 );
   newList.pushBack( 5 );
   newList.pushBack( 2 );
   newList.pushBack( 8 );

   List< int > addedElems;
   List< int > removedElems;
   ListUtils::compareWithPrevious( oldList, newList, addedElems, removedElems );

   CPPUNIT_ASSERT_EQUAL( (uint)2, addedElems.size() );
   CPPUNIT_ASSERT_EQUAL( (uint)1, removedElems.size() );

   List< int >::iterator addedElemsIt = addedElems.begin();
   CPPUNIT_ASSERT_EQUAL( 2, *addedElemsIt ); ++addedElemsIt;
   CPPUNIT_ASSERT_EQUAL( 8, *addedElemsIt );

   CPPUNIT_ASSERT_EQUAL( 6, removedElems.front() );
}

///////////////////////////////////////////////////////////////////////////////
