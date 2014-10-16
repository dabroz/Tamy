#include "core-TestFramework\TestFramework.h"


///////////////////////////////////////////////////////////////////////////////

static float frac( float val )
{
   int intVal = (int)val;
   return val - intVal;
}

static void EncodeDepth( float* outDepth, float depth, float maxZ )
{
   // encode
   float remappedDepth = depth / maxZ;
   outDepth[0] = frac( remappedDepth );
   outDepth[1] = frac( remappedDepth * 256.0f );

   // rasterize
   outDepth[0] = (unsigned char)( outDepth[0] * 256.0f );
   outDepth[1] = (unsigned char)( outDepth[1] * 256.0f );
}

static float DecodeDepth( float* encodedDepth, float maxZ )
{
   // derasterize
   float r = (unsigned char)encodedDepth[0] / 256.0f;
   float g = (unsigned char)encodedDepth[1] / 256.0f;

   // decode
   return ( r + g / 256.0f ) * maxZ;
}

static float pipeline( float z, float maxZ )
{
   float encodedDepth[2] = { 0, 0 };
   EncodeDepth( encodedDepth, z, maxZ );
   float decodedZ = DecodeDepth( encodedDepth, maxZ );
   return decodedZ;
}

///////////////////////////////////////////////////////////////////////////////

TEST( DepthEncoding, simple )
{
   float maxZ = 5000;

   for( float z = 0; z < 200; z += 0.1f )
   {
      float decodedZ = pipeline( z, maxZ );
      CPPUNIT_ASSERT_DOUBLES_EQUAL( z, decodedZ, 1e-1 );
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( DepthEncoding, testCases )
{
   float maxZ = 5000;
   float encodedDepth[2] = { 0, 0 };


   encodedDepth[0] = 0; encodedDepth[1] = 145;
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 11.06f, DecodeDepth( encodedDepth, maxZ ), 1e-1 );
   EncodeDepth( encodedDepth, 11.06f, maxZ );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, encodedDepth[0], 1e-1 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 144, encodedDepth[1], 1e-1 );

   encodedDepth[0] = 0; encodedDepth[1] = 255;
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 19.53f, DecodeDepth( encodedDepth, maxZ ), 1e-1 );
   encodedDepth[0] = 1; encodedDepth[1] = 0;
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 19.53f, DecodeDepth( encodedDepth, maxZ ), 1e-1 );
   
   encodedDepth[0] = 1; encodedDepth[1] = 145;
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 30.59f, DecodeDepth( encodedDepth, maxZ ), 1e-1 );
   EncodeDepth( encodedDepth, 30.59f, maxZ );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 1, encodedDepth[0], 1e-1 );
   CPPUNIT_ASSERT_DOUBLES_EQUAL( 144, encodedDepth[1], 1e-1 );
}

///////////////////////////////////////////////////////////////////////////////
