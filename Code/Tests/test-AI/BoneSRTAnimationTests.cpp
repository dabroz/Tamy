#include "core-TestFramework\TestFramework.h"
#include "core-AI\BoneSRTAnimation.h"


///////////////////////////////////////////////////////////////////////////////

TEST( BoneSRTAnimation, duration )
{
   BoneSRTAnimation anim;
   anim.addTranslationKey( 1.0f, Quad_0 );
   anim.addOrientationKey( 2.0f, Quaternion::IDENTITY );

   CPPUNIT_ASSERT_EQUAL( 2.0f, anim.m_duration );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BoneSRTAnimation, simpleTranslationAnim )
{
   BoneSRTAnimation anim;
   anim.addTranslationKey( 0, Quad_0 );
   anim.addTranslationKey( 1, Quad_1000 );

   CPPUNIT_ASSERT_EQUAL( 1.0f, anim.m_duration );

   Transform result;
   anim.sample( 0.0f, result );
   COMPARE_VEC( Vector( 0, 0, 0 ), result.m_translation );

   anim.sample( 1.0f, result );
   COMPARE_VEC( Vector( 1, 0, 0 ), result.m_translation );

   anim.sample( 0.5f, result );
   COMPARE_VEC( Vector( 0.5f, 0, 0 ), result.m_translation );

   // outside the boundaries access - return the boundary values, don't perform any looped lookups
   anim.sample( -0.1f, result );
   COMPARE_VEC( Vector( 0, 0, 0 ), result.m_translation );

   anim.sample( 1.1f, result );
   COMPARE_VEC( Vector( 1, 0, 0 ), result.m_translation );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BoneSRTAnimation, noKeysAnim )
{
   BoneSRTAnimation anim;

   CPPUNIT_ASSERT_EQUAL( 0.0f, anim.m_duration );

   Transform result;
   anim.sample( 0.0f, result );
   COMPARE_VEC( Vector_ZERO, result.m_translation );
   COMPARE_QUAT( Quaternion::IDENTITY, result.m_rotation );
}

///////////////////////////////////////////////////////////////////////////////

TEST( BoneSRTAnimation, replacingAnimKeys )
{
   BoneSRTAnimation anim;
   anim.addTranslationKey( 0, Vector( 0, 0, 0 ) );
   anim.addTranslationKey( 1, Vector( 1, 0, 0 ) );
   anim.addTranslationKey( 1, Vector( 2, 0, 0 ) );

   CPPUNIT_ASSERT_EQUAL( 1.0f, anim.m_duration );

   Transform result;
   anim.sample( 0.0f, result );
   COMPARE_VEC( Vector( 0, 0, 0 ), result.m_translation );

   anim.sample( 0.5f, result );
   COMPARE_VEC( Vector( 1.0f, 0, 0 ), result.m_translation );

   anim.sample( 1.0f, result );
   COMPARE_VEC( Vector( 2.0f, 0, 0 ), result.m_translation );
}

///////////////////////////////////////////////////////////////////////////////
