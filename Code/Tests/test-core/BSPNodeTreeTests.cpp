#include "core-TestFramework\TestFramework.h"
#include "core\Array.h"
#include "core\BSPNodeTree.h"
#include "core\Triangle.h"
#include "core\FastFloat.h"


///////////////////////////////////////////////////////////////////////////////

TEST(BSPNodeTree, singleTriangle)
{
   Array< Triangle* > geometry;
   Triangle* triangle = new Triangle(Vector(-1, 1, 0), Vector(1, 1, 0), Vector(-1, 0, 0));
   geometry.push_back(triangle);

   BSPNodeTree<Triangle> tree( geometry );

   // query the tree for triangles from various positions
   Array< Triangle* > queryResult;

   tree.query(Vector(0, 0, -1), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, queryResult.size());
   queryResult.clear();

   tree.query(Vector(0, 0, 1), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, queryResult.size());
   queryResult.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST(BSPNodeTree, multipleTrianglesNonSplitting)
{
   Array< Triangle* > geometry;
   Triangle* triangle1 = new Triangle(Vector(-1, 1, 1), Vector(1, 1, 1), Vector(-1, 0, 1));
   Triangle* triangle2 = new Triangle(Vector(-1, 1, -1), Vector(1, 1, -1), Vector(-1, 0, -1));
   geometry.push_back(triangle1);
   geometry.push_back(triangle2);

   BSPNodeTree<Triangle> tree( geometry );

   // query the tree for triangles from various positions
   Array<Triangle*> queryResult;

   tree.query(Vector(0, 0, 2), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, queryResult.size());
   CPPUNIT_ASSERT_EQUAL(triangle2, queryResult[0]);
   CPPUNIT_ASSERT_EQUAL(triangle1, queryResult[1]);
   queryResult.clear();

   tree.query(Vector(0, 0, 1), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, queryResult.size());
   CPPUNIT_ASSERT_EQUAL(triangle2, queryResult[0]);
   CPPUNIT_ASSERT_EQUAL(triangle1, queryResult[1]);
   queryResult.clear();

   tree.query(Vector(0, 0, 0), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, queryResult.size());
   CPPUNIT_ASSERT_EQUAL(triangle1, queryResult[0]);
   CPPUNIT_ASSERT_EQUAL(triangle2, queryResult[1]);
   queryResult.clear();

   tree.query(Vector(0, 0, -1), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, queryResult.size());
   CPPUNIT_ASSERT_EQUAL(triangle1, queryResult[0]);
   CPPUNIT_ASSERT_EQUAL(triangle2, queryResult[1]);
   queryResult.clear();

   tree.query(Vector(0, 0, -2), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, queryResult.size());
   CPPUNIT_ASSERT_EQUAL(triangle1, queryResult[0]);
   CPPUNIT_ASSERT_EQUAL(triangle2, queryResult[1]);
   queryResult.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST(BSPNodeTree, coplanarPolygons)
{
    Array<Triangle*> geometry;
    Triangle* triangle1 = new Triangle(Vector(-2, 1, 0), Vector(-1, 1, 0), Vector(-2, 0, 0));
    Triangle* triangle2 = new Triangle(Vector( 1, 1, 0), Vector( 2, 1, 0), Vector( 1, 0, 0));
    geometry.push_back(triangle1);
    geometry.push_back(triangle2);

    BSPNodeTree<Triangle> tree(geometry);

    // query the tree for triangles from various positions
    Array<Triangle*> queryResult;

    tree.query(Vector(0, 0, 1), queryResult);
    CPPUNIT_ASSERT_EQUAL((unsigned int)2, queryResult.size());
    CPPUNIT_ASSERT_EQUAL(triangle1, queryResult[0]);
    CPPUNIT_ASSERT_EQUAL(triangle2, queryResult[1]);
    queryResult.clear();

    tree.query(Vector(0, 0, -1), queryResult);
    CPPUNIT_ASSERT_EQUAL((unsigned int)2, queryResult.size());
    CPPUNIT_ASSERT_EQUAL(triangle1, queryResult[0]);
    CPPUNIT_ASSERT_EQUAL(triangle2, queryResult[1]);
    queryResult.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST(BSPNodeTree, intersectingPolygons)
{
   Array<Triangle*> geometry;
   Triangle* triangle1 = new Triangle(Vector(-1, 1, 0), Vector(1, 0, 0), Vector(-1, 0, 0));
   Triangle* triangle2 = new Triangle(Vector(0, 1, -1), Vector(0, 0, 1), Vector(0, 0, -1));
   geometry.push_back(triangle1);
   geometry.push_back(triangle2);

   BSPNodeTree<Triangle> tree(geometry);

   // query the tree for triangles from various positions
   Array<Triangle*> queryResult;

   Triangle triangle2_bk_1(Vector(0, 1, -1), Vector(0, 0.5f, 0), Vector(0, 0, 0));
   Triangle triangle2_bk_2(Vector(0, 1, -1), Vector(0, 0, 0), Vector(0, 0, -1));
   Triangle triangle2_fr_1(Vector(0, 0.5f, 0), Vector(0, 0, 1), Vector(0, 0, 0));

   tree.query(Vector(1, 0, 1), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)4, queryResult.size());
   CPPUNIT_ASSERT_EQUAL(triangle2_bk_1, *queryResult[0]);
   CPPUNIT_ASSERT_EQUAL(triangle2_bk_2, *queryResult[1]);
   CPPUNIT_ASSERT_EQUAL(*triangle1,     *queryResult[2]);
   CPPUNIT_ASSERT_EQUAL(triangle2_fr_1, *queryResult[3]);
   queryResult.clear();

   tree.query(Vector(1, 0, -1), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)4, queryResult.size());
   CPPUNIT_ASSERT_EQUAL(triangle2_fr_1, *queryResult[0]);
   CPPUNIT_ASSERT_EQUAL(*triangle1,     *queryResult[1]);
   CPPUNIT_ASSERT_EQUAL(triangle2_bk_1, *queryResult[2]);
   CPPUNIT_ASSERT_EQUAL(triangle2_bk_2, *queryResult[3]);
   queryResult.clear();

   tree.query(Vector(-1, 0, -1), queryResult);
   CPPUNIT_ASSERT_EQUAL((unsigned int)4, queryResult.size());
   CPPUNIT_ASSERT_EQUAL(triangle2_fr_1, *queryResult[0]);
   CPPUNIT_ASSERT_EQUAL(*triangle1,     *queryResult[1]);
   CPPUNIT_ASSERT_EQUAL(triangle2_bk_1, *queryResult[2]);
   CPPUNIT_ASSERT_EQUAL(triangle2_bk_2, *queryResult[3]);
   queryResult.clear();
}

///////////////////////////////////////////////////////////////////////////////
