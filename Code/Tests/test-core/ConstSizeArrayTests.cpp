#include "core-TestFramework\TestFramework.h"
#include "core\ConstSizeArray.h"


///////////////////////////////////////////////////////////////////////////////

TEST(ConstSizeArray, addingNewElements)
{
   ConstSizeArray<int> arr;

   CPPUNIT_ASSERT_EQUAL((unsigned int)0, arr.size());

   arr.insert(0);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, arr.size());
   CPPUNIT_ASSERT_EQUAL(0, arr[0]);

   arr.insert(1);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, arr.size());
   CPPUNIT_ASSERT_EQUAL(0, arr[0]);
   CPPUNIT_ASSERT_EQUAL(1, arr[1]);

   arr.insert(2);
   CPPUNIT_ASSERT_EQUAL((unsigned int)3, arr.size());
   CPPUNIT_ASSERT_EQUAL(0, arr[0]);
   CPPUNIT_ASSERT_EQUAL(1, arr.at(1));
   CPPUNIT_ASSERT_EQUAL(2, arr[2]);

   arr.insert(5);
   CPPUNIT_ASSERT_EQUAL((unsigned int)4, arr.size());
   CPPUNIT_ASSERT_EQUAL(0, arr[0]);
   CPPUNIT_ASSERT_EQUAL(1, arr.at(1));
   CPPUNIT_ASSERT_EQUAL(2, arr[2]);
   CPPUNIT_ASSERT_EQUAL(5, arr.at(3));
};

///////////////////////////////////////////////////////////////////////////////

TEST(ConstSizeArray, removingElements)
{
   ConstSizeArray<int> arr;

   arr.insert(0);
   arr.insert(1);
   arr.insert(2);
   arr.insert(3);

   CPPUNIT_ASSERT_EQUAL((unsigned int)4, arr.size());
   CPPUNIT_ASSERT_EQUAL(0, arr[0]);
   CPPUNIT_ASSERT_EQUAL(1, arr[1]);
   CPPUNIT_ASSERT_EQUAL(2, arr[2]);
   CPPUNIT_ASSERT_EQUAL(3, arr[3]);

   arr.remove(2);
   CPPUNIT_ASSERT_EQUAL((unsigned int)4, arr.size());
   CPPUNIT_ASSERT_EQUAL(0, arr[0]);
   CPPUNIT_ASSERT_EQUAL(1, arr[1]);
   CPPUNIT_ASSERT_EQUAL(3, arr[3]); // yes - indices don't change

   arr.remove(0);
   CPPUNIT_ASSERT_EQUAL((unsigned int)4, arr.size());
   CPPUNIT_ASSERT_EQUAL(1, arr[1]);
   CPPUNIT_ASSERT_EQUAL(3, arr[3]);

   arr.remove(1);
   CPPUNIT_ASSERT_EQUAL((unsigned int)4, arr.size());
   CPPUNIT_ASSERT_EQUAL(3, arr[3]);

   arr.remove(3);
   CPPUNIT_ASSERT_EQUAL((unsigned int)4, arr.size());
};

///////////////////////////////////////////////////////////////////////////////

TEST(ConstSizeArray, allocate)
{
   ConstSizeArray<int> arr;

   CPPUNIT_ASSERT_EQUAL((unsigned int)0, arr.size());

   arr.allocate(5);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, arr.size());

   arr.allocate(27);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, arr.size());

   // it doesn't work backwards
   arr.allocate(1);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, arr.size());
};

///////////////////////////////////////////////////////////////////////////////

TEST(ConstSizeArray, resize)
{
   ConstSizeArray<int> arr;

   CPPUNIT_ASSERT_EQUAL((unsigned int)0, arr.size());

   arr.resize(5);
   CPPUNIT_ASSERT_EQUAL((unsigned int)5, arr.size());

   arr.resize(27);
   CPPUNIT_ASSERT_EQUAL((unsigned int)27, arr.size());

   CPPUNIT_ASSERT_EQUAL(0, arr.at(22));

   arr.resize(1);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, arr.size());
};

///////////////////////////////////////////////////////////////////////////////

TEST(ConstSizeArray, initialSize)
{
   ConstSizeArray<int> arr1(5);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, arr1.size());

   ConstSizeArray<int> arr2(0);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, arr2.size());
};

///////////////////////////////////////////////////////////////////////////////

TEST(ConstSizeArray, finding)
{
   ConstSizeArray<int> arr;

   arr.insert(5);
   arr.insert(2);
   arr.insert(9);
   arr.insert(5); // second item with the same value
   arr.insert(6);

   CPPUNIT_ASSERT_EQUAL((unsigned int)1, arr.find(2));
   CPPUNIT_ASSERT_EQUAL((unsigned int)4, arr.find(6));
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, arr.find(5));
   CPPUNIT_ASSERT_EQUAL((unsigned int)3, arr.find(5, 1));
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, arr.find(9));

   CPPUNIT_ASSERT_EQUAL(EOA, arr.find(47));
};

///////////////////////////////////////////////////////////////////////////////

TEST(ConstSizeArray, iterator)
{
   ConstSizeArray<int> arr;

   arr.insert(5);
   arr.insert(2);
   arr.insert(9);
   arr.insert(5);
   arr.insert(6);

   // operator set on the array beginning
   ConstSizeArray<int>::iterator it = arr.begin();
   CPPUNIT_ASSERT_EQUAL(5, *it);


   // prefix incrementation operator
   it = arr.begin();
   CPPUNIT_ASSERT_EQUAL(2, *(++it));
   CPPUNIT_ASSERT_EQUAL(9, *(++it));
   CPPUNIT_ASSERT_EQUAL(5, *(++it));
   CPPUNIT_ASSERT_EQUAL(6, *(++it));

   // postfix incrementation operator
   it = arr.begin();
   CPPUNIT_ASSERT_EQUAL(5, *(it++));
   CPPUNIT_ASSERT_EQUAL(2, *(it++));
   CPPUNIT_ASSERT_EQUAL(9, *(it++));
   CPPUNIT_ASSERT_EQUAL(5, *(it++));
   CPPUNIT_ASSERT_EQUAL(6, *(it++));

   // prefix decrementation operator
   it = arr.end();
   CPPUNIT_ASSERT_EQUAL(6, *(--it));
   CPPUNIT_ASSERT_EQUAL(5, *(--it));
   CPPUNIT_ASSERT_EQUAL(9, *(--it));
   CPPUNIT_ASSERT_EQUAL(2, *(--it));
   CPPUNIT_ASSERT_EQUAL(5, *(--it));
   CPPUNIT_ASSERT_EQUAL(5, *it); // can't go back past the first element

   // postfix decrementation operator
   it = arr.end();
   it--;
   CPPUNIT_ASSERT_EQUAL(6, *(it--));
   CPPUNIT_ASSERT_EQUAL(5, *(it--));
   CPPUNIT_ASSERT_EQUAL(9, *(it--));
   CPPUNIT_ASSERT_EQUAL(2, *(it--));
   CPPUNIT_ASSERT_EQUAL(5, *(it--));

   // can't cross the start
   it = arr.begin();
   CPPUNIT_ASSERT_EQUAL(5, *(--it));

   // incrementation with removed middle element
   arr.remove(2);
   it = arr.begin();
   CPPUNIT_ASSERT_EQUAL(5, *(it++));
   CPPUNIT_ASSERT_EQUAL(2, *(it++));
   CPPUNIT_ASSERT_EQUAL(5, *(it++));
   CPPUNIT_ASSERT_EQUAL(6, *(it++));

   // decrementation with removed middle element
   it = arr.end();
   CPPUNIT_ASSERT_EQUAL(6, *(--it));
   CPPUNIT_ASSERT_EQUAL(5, *(--it));
   CPPUNIT_ASSERT_EQUAL(2, *(--it));
   CPPUNIT_ASSERT_EQUAL(5, *(--it));

   // creating boundary iterator when boundary elements were removed
   arr.remove(0);
   arr.remove(4);
   CPPUNIT_ASSERT_EQUAL(2, *(arr.begin()));

   // incrementation without boundary elements
   it = arr.begin();
   CPPUNIT_ASSERT_EQUAL(2, *(it++));
   CPPUNIT_ASSERT_EQUAL(5, *(it++));

   // decrementation without boundary elements
   it = arr.end();
   CPPUNIT_ASSERT_EQUAL(5, *(--it));
   CPPUNIT_ASSERT_EQUAL(2, *(--it));
   CPPUNIT_ASSERT_EQUAL(2, *(--it)); // stuck at the start
};

///////////////////////////////////////////////////////////////////////////////
