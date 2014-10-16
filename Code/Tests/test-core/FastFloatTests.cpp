#include "core-TestFramework\TestFramework.h"
#include "core\FastFloat.h"


///////////////////////////////////////////////////////////////////////////////

TEST( FastFloat, basicArithmetics )
{
   FastFloat a, b, c;

   a.setFromFloat( 1 );
   b.setFromFloat( 2 );

   c.setAdd( a, b );
   CPPUNIT_ASSERT_EQUAL( 3.0f , c.getFloat() ); 

   c.setSub( a, b );
   CPPUNIT_ASSERT_EQUAL( -1.0f , c.getFloat() ); 

   c.setMul( a, b );
   CPPUNIT_ASSERT_EQUAL( 2.0f , c.getFloat() ); 

   c.setDiv( a, b );
   CPPUNIT_ASSERT_EQUAL( 0.5f , c.getFloat() ); 

   {
      FastFloat d;
      d.setFromFloat( 5.0f );
      d.add( a );

      CPPUNIT_ASSERT_EQUAL( 6.0f , d.getFloat() ); 
   }

   {
      FastFloat d;
      d.setFromFloat( 5.0f );
      d.sub( a );

      CPPUNIT_ASSERT_EQUAL( 4.0f , d.getFloat() ); 
   }

   {
      FastFloat d;
      d.setFromFloat( 5.0f );
      d.mul( b );

      CPPUNIT_ASSERT_EQUAL( 10.0f , d.getFloat() ); 
   }

   {
      FastFloat d;
      d.setFromFloat( 5.0f );
      d.div( b );

      CPPUNIT_ASSERT_EQUAL( 2.5f , d.getFloat() ); 
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( FastFloat, booleanComparisons )
{
   FastFloat a, b;

   a.setFromFloat( 1 );
   b.setFromFloat( 2 );

  CPPUNIT_ASSERT( a < b );
  CPPUNIT_ASSERT( a <= b );
  CPPUNIT_ASSERT( a <= a );
  
  CPPUNIT_ASSERT( b > a );
  CPPUNIT_ASSERT( b >= a );
  CPPUNIT_ASSERT( b >= b );

  CPPUNIT_ASSERT( b != a );
  CPPUNIT_ASSERT_EQUAL( false, b == a );
  
  CPPUNIT_ASSERT( a == a );
  CPPUNIT_ASSERT( b == b );

}

///////////////////////////////////////////////////////////////////////////////

TEST( FastFloat, conversionFromPrimitiveTypes )
{
   FastFloat a;

   a.setFromFloat( 512.67f );
   CPPUNIT_ASSERT_EQUAL( 512.67f, a.getFloat() );

   a.setFromInt( 1024 );
   CPPUNIT_ASSERT_EQUAL( 1024.0f, a.getFloat() );

   a.setFromByte( 64 );
   CPPUNIT_ASSERT_EQUAL( 64.0f, a.getFloat() );

   a.setFromWord( 317 );
   CPPUNIT_ASSERT_EQUAL( 317.0f, a.getFloat() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( FastFloat, extraMethods )
{
   FastFloat posValue, negValue, largePosValue, d;
   posValue.setFromFloat( 3.0f );
   negValue.setFromFloat( -5.0f );
   largePosValue.setFromFloat( 10.0f );

   d.setMax( posValue, negValue );
   CPPUNIT_ASSERT_EQUAL( 3.0f, d.getFloat() );

   d.setMin( posValue, negValue );
   CPPUNIT_ASSERT_EQUAL( -5.0f, d.getFloat() );

   d.setClamped( largePosValue, negValue, posValue );  // 10 --> < -5, 3 >  == > 3
   CPPUNIT_ASSERT_EQUAL( 3.0f, d.getFloat() );

   d.setClamped( negValue, posValue, largePosValue );  // -5 --> < 3, 10 >  ==> 3
   CPPUNIT_ASSERT_EQUAL( 3.0f, d.getFloat() );

   d.setAbs( posValue );
   CPPUNIT_ASSERT_EQUAL( 3.0f, d.getFloat() );

   d.setAbs( negValue );
   CPPUNIT_ASSERT_EQUAL( 5.0f, d.getFloat() );

   d = negValue;
   d.abs();
   CPPUNIT_ASSERT_EQUAL( 5.0f, d.getFloat() );

   d.setFlipSign( posValue, negValue );
   CPPUNIT_ASSERT_EQUAL( -3.0f, d.getFloat() );

   d.setFlipSign( negValue, posValue );
   CPPUNIT_ASSERT_EQUAL( -5.0f, d.getFloat() );

   d.setSign( posValue );
   CPPUNIT_ASSERT_EQUAL( 1.0f, d.getFloat() );

   d.setSign( negValue );
   CPPUNIT_ASSERT_EQUAL( -1.0f, d.getFloat() );

   d = posValue;
   d.sign();
   CPPUNIT_ASSERT_EQUAL( 1.0f, d.getFloat() );

   d = negValue;
   d.sign();
   CPPUNIT_ASSERT_EQUAL( -1.0f, d.getFloat() );

   {
      FastFloat valA, valB, valC, eps;
      valA.setFromFloat( 6.0f );
      valB.setFromFloat( 5.999f );
      valC.setFromFloat( 6.001f );
      eps.setFromFloat( 1e-3 );

      CPPUNIT_ASSERT( valA.approxEqual( valB, eps ) );
      CPPUNIT_ASSERT( valA.approxEqual( valC, eps ) );
      CPPUNIT_ASSERT_EQUAL( false, valA.approxEqual( negValue, eps ) );
      CPPUNIT_ASSERT_EQUAL( false, valA.approxEqual( largePosValue, eps ) );
   }

   {
      FastFloat sqrVal;
      sqrVal.setFromFloat( 25.0f );

      d.setSqrt( sqrVal );
      CPPUNIT_ASSERT_EQUAL( 5.0f, d.getFloat() );

      d.setFromFloat( 49.0f );
      d.sqrt();
      CPPUNIT_ASSERT_EQUAL( 7.0f, d.getFloat() );
   }

   {
      d.setReciprocal( largePosValue );
      CPPUNIT_ASSERT_EQUAL( 0.1f, d.getFloat() );

      d = largePosValue;
      d.reciprocal();
      CPPUNIT_ASSERT_EQUAL( 0.1f, d.getFloat() );
   }

   {
      d.setNeg( posValue );
      CPPUNIT_ASSERT_EQUAL( -3.0f, d.getFloat() );

      d = posValue;
      d.neg();
      CPPUNIT_ASSERT_EQUAL( -3.0f, d.getFloat() );

      d.setNeg( negValue );
      CPPUNIT_ASSERT_EQUAL( 5.0f, d.getFloat() );

      d = negValue;
      d.neg();
      CPPUNIT_ASSERT_EQUAL( 5.0f, d.getFloat() );
   }
}

///////////////////////////////////////////////////////////////////////////////
